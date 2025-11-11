// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/connection.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/exceptions.hpp"
#include "astarte_device_sdk/mqtt/introspection.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "exponential_backoff.hpp"
#include "mqtt/async_client.h"
#include "mqtt/delivery_token.h"
#include "mqtt/exception.h"
#include "mqtt/iasync_client.h"
#include "mqtt/message.h"
#include "mqtt/token.h"

// All other necessary headers (spdlog, format, pairing.hpp, etc.)
// are included by "astarte_device_sdk/mqtt/connection.hpp"

namespace AstarteDeviceSdk {

//
// ConnectionCallback Implementation
//

void ConnectionCallback::setup_subscriptions() {
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

void ConnectionCallback::send_introspection() {
  // create the stringified representation of the introspection to send to Astarte
  auto introspection_str = std::string();
  for (auto interface : introspection_) {
    introspection_str += std::format("{}:{}:{};", interface.interface_name, interface.version_major,
                                     interface.version_minor);
  }
  // remove last unnecessary ";"
  introspection_str.pop_back();

  auto base_topic = std::format("testrg/{}", device_id_);
  client_->publish(base_topic, introspection_str, 2, false);
}

void ConnectionCallback::send_emptycache() {
  auto emptycache_topic = std::format("/{}/emptyCache", device_id_);
  client_->publish(emptycache_topic, "1", 2, false);
}

void ConnectionCallback::reconnect() {
  ExponentialBackoff backoff(std::chrono::seconds(2), std::chrono::minutes(1));

  while (!client_->is_connected()) {
    try {
      // TODO(rgwork): call exponential backoff inside reconnect
      client_->connect(options_, nullptr, *this);
    } catch (const mqtt::exception& e) {
      spdlog::error("error while trying to reconnect to Astarte: {}", e.what());
    }

    auto delay = backoff.getNextDelay();
    spdlog::info("will attempt to reconnect in {} seconds.",
                 std::chrono::duration_cast<std::chrono::seconds>(delay).count());
    std::this_thread::sleep_for(delay);
  };
}

void ConnectionCallback::connected(const std::string& /* cause */) {
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

void ConnectionCallback::connection_lost(const std::string& cause) {
  spdlog::warn("connection lost: {}, reconnecting...", cause);
  reconnect();
}

void ConnectionCallback::message_arrived(mqtt::const_message_ptr msg) {
  // TODO(rgwork): handle message reception
  spdlog::trace("message received at {}: {}", msg->get_topic(), msg->to_string());
}

void ConnectionCallback::delivery_complete(mqtt::delivery_token_ptr token) {}

void ConnectionCallback::on_failure(const mqtt::token& /* tok */) {
  spdlog::error("failed to reconnect, retrying...");
  reconnect();
}

void ConnectionCallback::on_success(const mqtt::token& tok) {}

ConnectionCallback::ConnectionCallback(mqtt::iasync_client* client, mqtt::connect_options options,
                                       std::string device_id, std::vector<Interface>& introspection)
    : client_(client),
      options_(std::move(options)),
      device_id_(std::move(device_id)),
      introspection_(introspection) {}

MqttConnection::MqttConnection(MqttConfig cfg) : cfg_(std::move(cfg)) {
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

void MqttConnection::connect(std::vector<Interface>& introspection) {
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

void MqttConnection::disconnect() {
  try {
    auto toks = client_->get_pending_delivery_tokens();
    if (!toks.empty()) {
      spdlog::error("Error: There are pending delivery tokens!");
    }

    spdlog::debug("disconnecting device from astarte...");
    client_->disconnect()->wait();
    spdlog::info("device disconnected from Astarte");
  } catch (const mqtt::exception& e) {
    throw MqttConnectionException(
        std::format("Mqtt disconnection error (ID {}): {}", e.get_reason_code(), e.what()));
  }
}

}  // namespace AstarteDeviceSdk
