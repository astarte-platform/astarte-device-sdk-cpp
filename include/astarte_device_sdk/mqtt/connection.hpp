// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_H
#define ASTARTE_MQTT_CONNECTION_H

#include <spdlog/spdlog.h>

#include <format>
#include <string_view>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "astarte_device_sdk/mqtt/introspection.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "mqtt/async_client.h"
#include "mqtt/iasync_client.h"

namespace AstarteDeviceSdk {

namespace {

/**
 * @brief Retrieve and persist device crypto credentials.
.
 * @param api The PairingApi instance used to request the credentials.
 * @param secret The credential secret used to authenticate the request.
 * @param store_dir The directory path where the certificate and private key files will be created.
 * @return an error if the API request fails or file writing errors occur.
*/
auto setup_crypto_files(PairingApi& api, const std::string_view secret,
                        const std::string_view store_dir)
    -> astarte_tl::expected<void, AstarteError> {
  auto key_cert_res = api.get_device_key_and_cert(secret);
  if (!key_cert_res) {
    return astarte_tl::unexpected(key_cert_res.error());
  }

  auto [client_priv_key, client_cert] = key_cert_res.value();

  const std::vector<std::pair<std::string, std::string>> files = {
      {std::format("{}/{}", store_dir, CLIENT_CERTIFICATE_FILE), client_cert},
      {std::format("{}/{}", store_dir, PRIVATE_KEY_FILE), client_priv_key}};

  for (const auto& [path, content] : files) {
    auto write_res = write_to_file(path, content);
    if (!write_res) {
      spdlog::error("Failed to write to {}. Error: {}", path, write_res.error());
      return astarte_tl::unexpected(write_res.error());
    }
  }

  return {};
}

}  // namespace

/**
 * @brief The MQTT topic for Astarte control consumer properties.
 */
constexpr std::string_view MQTT_CONTROL_CONSUMER_PROP_TOPIC = "/control/consumer/properties";

/**
 * @brief Implement MQTT callbacks for handling connection events.
 */
class ConnectionCallback : public virtual mqtt::callback, public virtual mqtt::iaction_listener {
  /**
   * @brief Subscribe the client to all required Astarte topics.
   *
   * This includes the control topic and all topics for server-owned
   * interfaces defined in the device's introspection.
   */
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

  /**
   * @brief Publishe the device's introspection to Astarte.
   */
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

  /**
   * @brief Send an "emptyCache" message to Astarte.
   */
  void send_emptycache() {
    auto emptycache_topic = std::format("/{}/emptyCache", device_id_);
    client_->publish(emptycache_topic, "1", 2, false);
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
  }

  // Callback for when a message arrives.
  void message_arrived(mqtt::const_message_ptr msg) override {
    // TODO: handle message reception
    spdlog::trace("message received at {}: {}", msg->get_topic(), msg->to_string());
  }

  /**
   * @brief Called when a message delivery is complete.
   * @param token The delivery token associated with the message.
   */
  void delivery_complete(mqtt::delivery_token_ptr token) override {}

  // Re-connection failure
  void on_failure(const mqtt::token& tok) override {
    spdlog::error("failed to reconnect, retrying...");
  }

  // (Re)connection success
  void on_success(const mqtt::token& tok) override {}

 public:
  /**
   * @brief Construct a new Connection Callback object.
   *
   * @param client Pointer to the MQTT asynchronous client.
   * @param options The MQTT Paho connection options.
   * @param device_id The Astarte Device ID.
   * @param introspection A reference to the vector of device interfaces.
   */
  ConnectionCallback(mqtt::iasync_client* client, mqtt::connect_options options,
                     std::string device_id, std::vector<Interface>& introspection)
      : client_(client), options_(options), device_id_(device_id), introspection_(introspection) {}

  /// @brief Pointer to the MQTT client, used for operations like subscribe.
  mqtt::iasync_client* client_;
  /// @brief MQTT connection options, used for reconnection.
  mqtt::connect_options options_;
  /// @brief The Astarte Device ID.
  std::string device_id_;
  /// @brief Reference to the device's introspection (list of interfaces).
  std::vector<Interface>& introspection_;
};

/**
 * @brief Manage the MQTT connection to an Astarte instance.
 */
class MqttConnection {
 public:
  /** @brief Copy constructor for the MqttConnection class. */
  MqttConnection(const MqttConnection&) = delete;
  /** @brief Copy assignment operator for the MqttConnection class. */
  MqttConnection& operator=(const MqttConnection&) = delete;
  /** @brief Move constructor for the MqttConnection class. */
  MqttConnection(MqttConnection&&) = default;
  /** @brief Move assignment operator for the MqttConnection class. */
  MqttConnection& operator=(MqttConnection&&) = default;

  /**
   * @brief Construct a new Mqtt Connection object.
   *
   * Initialize the connection parameters by performing pairing with the Astarte
   * instance specified in the configuration. It retrieves the broker URL and
   * sets up the MQTT client options.
   *
   * @param cfg The MQTT configuration object containing connection details.
   * @return The MQTT connection object, an error otherwise.
   */
  static auto create(MqttConfig cfg) -> astarte_tl::expected<MqttConnection, AstarteError> {
    auto realm = cfg.realm();
    auto device_id = cfg.device_id();
    auto pairing_url = cfg.pairing_url();

    auto credential_secret = cfg.read_secret_or_register();
    if (!credential_secret) {
      spdlog::error("failed to read credential secret or register the device. Error: {}",
                    credential_secret.error());
      return astarte_tl::unexpected(credential_secret.error());
    }

    auto res = PairingApi::create(realm, device_id, pairing_url);
    if (!res) {
      spdlog::error("failed to create PairingApi instance. Error: {}", res.error());
      return astarte_tl::unexpected(res.error());
    }
    auto api = res.value();

    auto broker_url = api.get_broker_url(credential_secret.value());
    if (!broker_url) {
      spdlog::error("failed to retrieve Astarte MQTT broker URL. Error: {}", broker_url.error());
      return astarte_tl::unexpected(broker_url.error());
    }

    auto crypto_setup = setup_crypto_files(api, credential_secret.value(), cfg.store_dir());
    if (!crypto_setup) {
      spdlog::error("failed to setup crypto info. Error: {}", crypto_setup.error());
      return astarte_tl::unexpected(crypto_setup.error());
    }

    auto options = cfg.build_mqtt_options();
    if (!options) {
      spdlog::error("failed to build Astarte MQTT options. Error: {}", options.error());
      return astarte_tl::unexpected(options.error());
    }

    auto client_id = std::format("{}/{}", realm, device_id);
    auto client = std::make_unique<mqtt::async_client>(broker_url.value(), client_id);

    return MqttConnection(std::move(cfg), std::move(options.value()), std::move(client));
  }

  /**
   * @brief Connect the client to the Astarte MQTT broker.
   * @param introspection A vector of interfaces defining the device.
   * @return an error if the connection operation fails.
   */
  auto connect(std::vector<Interface>& introspection) -> astarte_tl::expected<void, AstarteError> {
    try {
      spdlog::debug("setting up connection callback...");
      cb_ = std::make_unique<ConnectionCallback>(client_.get(), options_,
                                                 std::string(cfg_.device_id()), introspection);
      client_->set_callback(*cb_);

      spdlog::debug("connecting device to the Astarte MQTT broker...");
      client_->connect(options_)->wait();
    } catch (const mqtt::exception& e) {
      spdlog::error("error while trying to connect to Astarte: {}", e.what());
      return astarte_tl::unexpected(AstarteMqttConnectionError(
          astarte_fmt::format("Mqtt connection error (ID {}): {}", e.get_reason_code(), e.what())));
    }

    spdlog::info("device connected to Astarte");
    return {};
  }

  /**
   * @brief Disconnect the client from the Astarte MQTT broker.
   * @return an error if the disconnection operation fails.
   */
  auto disconnect() -> astarte_tl::expected<void, AstarteError> {
    try {
      auto toks = client_->get_pending_delivery_tokens();
      if (!toks.empty()) spdlog::error("Error: There are pending delivery tokens!");

      spdlog::debug("disconnecting device from astarte...");
      client_->disconnect()->wait();
    } catch (const mqtt::exception& e) {
      return astarte_tl::unexpected(AstarteMqttConnectionError(astarte_fmt::format(
          "Mqtt disconnection error (ID {}): {}", e.get_reason_code(), e.what())));
    }

    spdlog::info("device disconnected from Astarte");
    return {};
  }

 private:
  MqttConnection(MqttConfig cfg, mqtt::connect_options options,
                 std::unique_ptr<mqtt::async_client> client)
      : cfg_(std::move(cfg)), options_(std::move(options)), client_(std::move(client)) {}

  /// @brief The MQTT configuration object.
  MqttConfig cfg_;
  /// @brief The Paho MQTT connection options.
  mqtt::connect_options options_;
  /// @brief The underlying Paho MQTT async client.
  std::unique_ptr<mqtt::async_client> client_;
  /// @brief The callback handler for MQTT events.
  std::unique_ptr<ConnectionCallback> cb_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONNECTION_H
