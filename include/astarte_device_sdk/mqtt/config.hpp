// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONFIG_H
#define ASTARTE_MQTT_CONFIG_H

/**
 * @file config.hpp
 * @brief Defines classes and parameters for configuring an MQTT connection to Astarte.
 *
 * This file provides the MqttConfig and Credential classes, which are used to set up all
 * necessary parameters for device authentication and communication with the Astarte
 * platform over the MQTT transport.
 */

#include <optional>
#include <string>
#include <string_view>

/** @brief Umbrella namespace for the Astarte C++ Device SDK. */
namespace AstarteDeviceSdk {

/** @brief Default keep alive interval in seconds for the MQTT connection. */
constexpr int DEFAULT_KEEP_ALIVE = 30;

/** @brief Default connection timeout in seconds for the MQTT connection. */
constexpr int DEFAULT_CONNECTION_TIMEOUT = 5;

/**
 * @brief Configuration for the Astarte MQTT connection.
 * @details This class holds all necessary settings for connecting a device to Astarte via MQTT.
 * It uses a builder pattern, allowing for fluent and readable configuration.
 */
class MqttConfig {
 public:
  /**
   * @brief Constructor to enforce object creation via static factory methods.
   * @param realm The Astarte realm the device belongs to.
   * @param device_id The unique identifier for the device.
   * @param credential_secret The credential secret for this device.
   * @param pairing_url The base URL of the Astarte Pairing API.
   * @param store_dir Path to a local directory for persisting certificates and state.
   */
  MqttConfig(std::string_view realm, std::string_view device_id, std::string_view credential_secret,
             std::string_view pairing_url, std::string_view store_dir)
      : realm_(realm),
        device_id_(device_id),
        credential_secret_(credential_secret),
        pairing_url_(pairing_url),
        store_dir_(store_dir),
        ignore_ssl_(false),
        keepalive_(DEFAULT_KEEP_ALIVE),
        conn_timeout_(DEFAULT_CONNECTION_TIMEOUT) {}

  /**
   * @brief Get the configured realm.
   * @return a string containing the realm
   */
  auto realm() -> std::string_view;

  /**
   * @brief Get the configured device ID.
   * @return a string containing the device ID
   */
  auto device_id() -> std::string_view;

  /**
   * @brief Get the configured Pairing API URL.
   * @return a string containing the pairing URL
   */
  auto pairing_url() -> std::string_view;

  /**
   * @brief Get the configured local storage directory.
   * @return a string containing the store directory
   */
  auto store_dir() -> std::string_view;

  /**
   * @brief Get the configured credential secret.
   * @return a string containing the credential secret
   */
  auto credential_secret() -> std::string_view;

  /**
   * @brief Set the MQTT keep-alive interval.
   * @param duration The keep-alive duration in seconds.
   * @return A reference to the MqttConfig object for chaining.
   */
  auto keepalive(int duration) -> MqttConfig&;

  /**
   * @brief Configure the client to ignore TLS/SSL certificate validation errors.
   * @return A reference to the MqttConfig object for chaining.
   */
  auto ignore_ssl_errors() -> MqttConfig&;

  /**
   * @brief Set the MQTT connection timeout.
   * @param duration The timeout duration in seconds.
   * @return A reference to the MqttConfig object for chaining.
   */
  auto connection_timeout(int duration) -> MqttConfig&;

 private:
  // The Astarte realm.
  std::string realm_;
  // The unique device identifier.
  std::string device_id_;
  // The base URL for the Pairing API.
  std::string pairing_url_;
  // The device's authentication credential secret.
  std::string credential_secret_;
  // The local directory for storing persistent data.
  std::string store_dir_;
  // Flag to ignore SSL/TLS certificate errors.
  bool ignore_ssl_;
  // Keep-alive interval in seconds.
  int keepalive_;
  // Connection timeout in seconds.
  int conn_timeout_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONFIG_H
