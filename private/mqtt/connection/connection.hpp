// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_H
#define ASTARTE_MQTT_CONNECTION_H

#include <spdlog/spdlog.h>

#include <atomic>
#include <format>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "mqtt/connection/callbacks.hpp"
#include "mqtt/connection/listener.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "mqtt/async_client.h"
#include "mqtt/iasync_client.h"
#include "mqtt/introspection.hpp"

namespace AstarteDeviceSdk {

/**
 * @brief Manages the MQTT connection to an Astarte instance.
 */
class MqttConnection {
 public:
  /** @brief Copy constructor for the MqttConnection class. */
  MqttConnection(const MqttConnection&) = delete;
  /** @brief Move constructor for the MqttConnection class. */
  MqttConnection(MqttConnection&&) = default;
  /** @brief Copy assignment operator for the MqttConnection class. */
  MqttConnection& operator=(const MqttConnection&) = delete;
  /**
   * @brief Move assignment operator for the MqttConnection class.
   * @return A reference to this MqttConnection object.
   */
  MqttConnection& operator=(MqttConnection&&) = default;

  /**
   * @brief Construct a new Mqtt Connection object.
   *
   * Initializes connection parameters, performs pairing if necessary,
   * and configures the MQTT client.
   *
   * @param cfg The MQTT configuration object containing connection details.
   * @return The MQTT connection object, an error otherwise.
   */
  static auto create(config::MqttConfig& cfg) -> astarte_tl::expected<MqttConnection, AstarteError>;

  /**
   * @brief Connects the client to the Astarte MQTT broker.
   * @param introspection A collection of interfaces defining the device.
   * @return an error if the connection operation fails.
   */
  auto connect(std::shared_ptr<Introspection> introspection)
      -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Check if the device is connected.
   * @return True if the device is connected to Astarte, false otherwise.
   */
  [[nodiscard]] auto is_connected() const -> bool;

  /**
   * @brief Send an individual or object data to Astarte.
   * @param interface_name The interface on which data will be sent.
   * @param path The mapping path of the Astarte interface on which data will be sentr.
   * @param qos The quality of service value. It could be only 0, 1 or 2.
   * @param data A vector of bytes containing the BSON data to send to Astarte.
   * @return an error if sending failed, nothing otherwise.
   */
  auto send(std::string_view interface_name, std::string_view path, uint8_t qos,
            const std::span<uint8_t> data) -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Disconnect the client from the Astarte MQTT broker.
   * @return an error if the disconnection operation fails.
   */
  auto disconnect(std::chrono::milliseconds timeout) -> astarte_tl::expected<void, AstarteError>;

 private:
  MqttConnection(config::MqttConfig cfg, mqtt::connect_options options,
                 std::unique_ptr<mqtt::async_client> client);

  /// @brief The MQTT configuration object.
  config::MqttConfig cfg_;
  /// @brief The Paho MQTT connection options.
  mqtt::connect_options options_;
  /// @brief The underlying Paho MQTT async client.
  std::unique_ptr<mqtt::async_client> client_;
  /// @brief The callback handler for MQTT events.
  std::unique_ptr<MqttConnectionCallback> cb_;

  /**
   * @brief The flag stating if the device is successfully connected to Astarte.
   *
   * The connection is correctly established to astarte after.
   */
  std::shared_ptr<std::atomic<bool>> connected_;

  /// @brief Queue containing the token used during connection
  std::shared_ptr<mqtt::thread_queue<mqtt::token_ptr>> connection_tokens_;

};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONNECTION_H
