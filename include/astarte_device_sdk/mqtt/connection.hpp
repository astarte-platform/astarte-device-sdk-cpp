// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_H
#define ASTARTE_MQTT_CONNECTION_H

#include <spdlog/spdlog.h>

#include <format>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/exceptions.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "mqtt/async_client.h"

namespace AstarteDeviceSdk {

/**
 * @brief Manages the MQTT connection to an Astarte instance.
 */
class MqttConnection {
 public:
  /**
   * @brief Constructs a new Mqtt Connection object.
   *
   * Initializes the connection parameters by performing pairing with the Astarte
   * instance specified in the configuration. It retrieves the broker URL and
   * sets up the MQTT client options.
   *
   * @param cfg The MQTT configuration object containing connection details.
   */
  MqttConnection(MqttConfig cfg) : cfg_(std::move(cfg)) {
    auto realm = cfg_.realm();
    auto device_id = cfg_.device_id();
    auto pairing_url = cfg_.pairing_url();
    auto secret = cfg_.read_secret_or_register();

    auto api = PairingApi(realm, device_id, pairing_url);
    auto broker_url = api.get_broker_url(secret);
    auto client_id = std::format("{}/{}", realm, device_id);

    options_ = cfg_.build_mqtt_options();
    client_ = std::make_unique<mqtt::async_client>(broker_url, client_id);
  }

  /**
   * @brief Connects the client to the Astarte MQTT broker.
   * @throws AstarteDeviceSdk::MqttConnectionException if the connection fails.
   */
  void connect() {
    try {
      spdlog::debug("connecting client to the Astarte MQTT broker...");
      client_->connect(options_)->wait();
      spdlog::info("client connected to Astarte");
    } catch (const mqtt::exception& e) {
      throw MqttConnectionException(
          std::format("Mqtt connection error (ID {}): {}", e.get_reason_code(), e.what()));
    }
  }

  /**
   * @brief Disconnects the client from the Astarte MQTT broker.
   * @throws AstarteDeviceSdk::MqttConnectionException if an error occurs during disconnection.
   */
  void disconnect() {
    try {
      spdlog::debug("disconnecting device from astarte...");
      client_->disconnect()->wait();
      spdlog::info("device disconnected from Astarte");
    } catch (const mqtt::exception& e) {
      throw MqttConnectionException(
          std::format("Mqtt disconnection error (ID {}): {}", e.get_reason_code(), e.what()));
    }
  }

 private:
  MqttConfig cfg_;
  mqtt::connect_options options_;
  std::unique_ptr<mqtt::async_client> client_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONNECTION_H
