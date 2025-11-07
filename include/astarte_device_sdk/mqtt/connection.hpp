// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_H
#define ASTARTE_MQTT_CONNECTION_H

#include <spdlog/spdlog.h>

#include <format>
#include <string_view>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/exceptions.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "astarte_device_sdk/mqtt/introspection.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "mqtt/async_client.h"
#include "mqtt/iasync_client.h"

namespace AstarteDeviceSdk {

constexpr std::string_view MQTT_CONTROL_CONSUMER_PROP_TOPIC = "/control/consumer/properties";

class ConnectionCallback : public virtual mqtt::callback, public virtual mqtt::iaction_listener {
  void setup_subscriptions() {
    // define a collection of topics to subscribe to
    auto topics = mqtt::string_collection();
    auto qoss = mqtt::iasync_client::qos_collection();

    spdlog::debug("subscribing to topic {}", MQTT_CONTROL_CONSUMER_PROP_TOPIC);
    topics.push_back(std::string(MQTT_CONTROL_CONSUMER_PROP_TOPIC));
    qoss.push_back(2);

    for (auto interface : introspection_) {
      // consider only server-owned properties
      if (interface.ownership == AstarteOwnership::kDevice) {
        continue;
      }

      auto topic = std::format("/{}/{}/#", device_id_, interface.interface_name);
      spdlog::debug("subscribing to topic {}", topic);
      topics.push_back(std::move(topic));
      qoss.push_back(2);
    }

    client_->subscribe(std::make_shared<mqtt::string_collection>(topics), qoss);
    spdlog::info("subscribed to Astarte topics");
  }

  void send_introspection() {
    // create the stringified representation of the introspection to send to Astarte
    auto introspection_str = std::string();
    for (auto i : introspection_) {
      introspection_str +=
          std::format("{}:{}:{};", i.interface_name, i.version_major, i.version_minor);
    }
    // remove last unnecessary ";"
    introspection_str.pop_back();

    auto base_topic = std::format("testrg/{}", device_id_);
    client_->publish(base_topic, introspection_str, 2, false);
  }

  void send_emptycache() {
    auto emptycache_topic = std::format("/{}/emptyCache", device_id_);
    client_->publish(emptycache_topic, "1", 2, false);
  }

  void reconnect() {
    try {
      // TODO: call exponential backoff inside reconnect
      client_->connect(options_, nullptr, *this);
    } catch (const mqtt::exception& e) {
      spdlog::error("error while trying to reconnect to Astarte: {}", e.what());
      throw MqttConnectionException(
          std::format("Mqtt reconnection error (ID {}): {}", e.get_reason_code(), e.what()));
    }
  }

  // (Re)connection success
  void connected(const std::string& cause) override {
    spdlog::info("device connected to Astarte");

    spdlog::debug("setting up subscription to Astarte topics...");
    setup_subscriptions();
    spdlog::info("subscription to Astarte topics completed");

    spdlog::debug("sending introspection to Astarte...");
    send_introspection();
    spdlog::info("introspection sent to Astarte");

    spdlog::debug("sending emptycache to Astarte...");
    send_emptycache();
    spdlog::debug("emptycache sent to Astarte");
  }

  // Callback for when the connection is lost.
  // This will initiate the attempt to manually reconnect.
  void connection_lost(const std::string& cause) override {
    spdlog::warn("connection lost: {}, reconnecting...", cause);
    reconnect();
  }

  // Callback for when a message arrives.
  void message_arrived(mqtt::const_message_ptr msg) override {
    // TODO: handle message reception
    spdlog::trace("message received at {}: {}", msg->get_topic(), msg->to_string());
  }

  void delivery_complete(mqtt::delivery_token_ptr token) override {}

  // Re-connection failure
  void on_failure(const mqtt::token& tok) override {
    spdlog::error("failed to reconnect, retrying...");
    reconnect();
  }

  // (Re)connection success
  void on_success(const mqtt::token& tok) override {}

 public:
  ConnectionCallback(mqtt::iasync_client* client, mqtt::connect_options options,
                     std::string device_id, std::vector<Interface>& introspection)
      : client_(client), options_(options), device_id_(device_id), introspection_(introspection) {}

  mqtt::iasync_client* client_;
  mqtt::connect_options options_;
  std::string device_id_;
  std::vector<Interface>& introspection_;
};

/**
 * @brief Manage the MQTT connection to an Astarte instance.
 */
class MqttConnection {
 public:
  /**
   * @brief Construct a new Mqtt Connection object.
   *
   * Initialize the connection parameters by performing pairing with the Astarte
   * instance specified in the configuration. It retrieves the broker URL and
   * sets up the MQTT client options.
   *
   * @param cfg The MQTT configuration object containing connection details.
   */
  MqttConnection(MqttConfig cfg) : cfg_(std::move(cfg)) {
    auto realm = cfg_.realm();
    auto device_id = cfg_.device_id();
    auto pairing_url = cfg_.pairing_url();
    auto credential_secret = cfg_.read_secret_or_register();
    auto store_dir = cfg_.store_dir();

    auto api = PairingApi(realm, device_id, pairing_url);
    auto broker_url = api.get_broker_url(credential_secret);
    auto client_id = std::format("{}/{}", realm, device_id);

    auto [client_priv_key, client_cert] = api.get_device_key_and_cert(credential_secret);

    auto client_cert_dir = std::format("{}/{}", store_dir, CLIENT_CERTIFICATE_FILE);
    auto client_priv_key_dir = std::format("{}/{}", store_dir, PRIVATE_KEY_FILE);

    write_to_file(client_cert_dir, client_cert);
    write_to_file(client_priv_key_dir, client_priv_key);

    options_ = cfg_.build_mqtt_options();
    client_ = std::make_unique<mqtt::async_client>(broker_url, client_id);
  }

  /**
   * @brief Connect the client to the Astarte MQTT broker.
   * @throws AstarteDeviceSdk::MqttConnectionException if the connection fails.
   */
  void connect(std::vector<Interface>& introspection) {
    try {
      spdlog::debug("setting up connection callback...");
      cb_ = std::make_unique<ConnectionCallback>(client_.get(), options_,
                                                 std::string(cfg_.device_id()), introspection);
      client_->set_callback(*cb_);

      spdlog::debug("connecting device to the Astarte MQTT broker...");
      client_->connect(options_)->wait();
    } catch (const mqtt::exception& e) {
      spdlog::error("error while trying to connect to Astarte: {}", e.what());
      throw MqttConnectionException(
          std::format("Mqtt connection error (ID {}): {}", e.get_reason_code(), e.what()));
    }
  }

  /**
   * @brief Disconnect the client from the Astarte MQTT broker.
   * @throws AstarteDeviceSdk::MqttConnectionException if an error occurs during disconnection.
   */
  void disconnect() {
    try {
      auto toks = client_->get_pending_delivery_tokens();
      if (!toks.empty()) spdlog::error("Error: There are pending delivery tokens!");

      spdlog::debug("disconnecting device from astarte...");
      client_->disconnect()->wait();
      spdlog::info("device disconnected from Astarte");
    } catch (const mqtt::exception& e) {
      throw MqttConnectionException(
          std::format("Mqtt disconnection error (ID {}): {}", e.get_reason_code(), e.what()));
    }
  }

 private:
  /// @brief The MQTT configuration object.
  MqttConfig cfg_;
  /// @brief The Paho MQTT connection options.
  mqtt::connect_options options_;
  /// @brief The underlying Paho MQTT async client.
  std::unique_ptr<mqtt::async_client> client_;
  std::unique_ptr<ConnectionCallback> cb_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONNECTION_H
