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

    // TODO: before building the mqtt options we must save the client certificate and private key to
    // file
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
  void connect() {
    try {
      spdlog::debug("connecting device to the Astarte MQTT broker...");
      client_->connect(options_)->wait();
      spdlog::info("device connected to Astarte");
    } catch (const mqtt::exception& e) {
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
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONNECTION_H
