// (C) Copyright 2025, SECO Mind Srl
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
 * @brief Implements `mqtt::callback` to handle connection life-cycle events and session setup.
 *
 * This class owns the device state (ID, introspection) and is responsible for
 * subscribing to topics and publishing introspection when a session is established.
 */
class MqttConnectionCallback : public virtual mqtt::callback {
 public:
  /**
   * @brief Construct a new Connection Callback object.
   *
   * @param client Pointer to the MQTT asynchronous client.
   * @param realm The Astarte realm name.
   * @param device_id The Astarte Device ID.
   * @param introspection A reference to the collection of device interfaces.
   * @param connected A flag stating if the client is correctly connected to Astarte.
   * @param handshake_error A flag stating if an error occurred while attempting to connect to
   * Astarte.
   */
  MqttConnectionCallback(mqtt::iasync_client* client, std::string realm, std::string device_id,
                         std::shared_ptr<Introspection> introspection,
                         std::shared_ptr<std::atomic<bool>> connected);

  /**
   * @brief Performs the Astarte session setup.
   *
   * This includes subscribing to control/data topics and sending the introspection
   * and emptyCache messages.
   *
   * @param session_present Indicates if the broker resumed a previous session.
   * If true, subscriptions might be skipped depending on logic.
   * @return an error if the operation fails
   */
  auto perform_session_setup(bool session_present) -> astarte_tl::expected<void, AstarteError>;

 private:
  /**
   * @brief Subscribes the client to all required Astarte topics.
   *
   * Includes the control topic and all topics for server-owned interfaces.
   * @return an error if the operation fails
   */
  auto setup_subscriptions() -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Publishes the device's introspection to Astarte.
   * @return an error if the operation fails
   */
  auto send_introspection() -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Sends an "emptyCache" message to Astarte.
   * @return an error if the operation fails
   */
  auto send_emptycache() -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Called by the client when the connection is established (e.g., after auto-reconnect).
   * @param cause The cause of the connection (e.g., "automatic reconnect").
   */
  void connected(const std::string& cause) override;

  /**
   * @brief Called when the connection is lost.
   * @param cause The reason for the disconnection.
   */
  void connection_lost(const std::string& cause) override;

  /**
   * @brief Called when a message arrives from the broker.
   * @param msg The received message.
   */
  void message_arrived(mqtt::const_message_ptr msg) override;

  /**
   * @brief Called when a message delivery is complete.
   * @param token The delivery token associated with the message.
   */
  void delivery_complete(mqtt::delivery_token_ptr token) override;

  /// @brief Pointer to the MQTT client, used for operations like subscribe.
  mqtt::iasync_client* client_;
  /// @brief The Astarte Realm name.
  std::string realm_;
  /// @brief The Astarte Device ID.
  std::string device_id_;
  /// @brief Reference to the device's introspection.
  std::shared_ptr<Introspection> introspection_;
  /// @brief The flag stating if the device is successfully connected to Astarte.
  std::shared_ptr<std::atomic<bool>> connected_;
  /// @brief The flag stating if an error occurred while attempting to connect to Astarte.
  std::shared_ptr<std::atomic<bool>> handshake_error_;
};

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
   * @brief Send an individual data to Astarte.
   * @param interface_name A collection of interfaces defining the device.
   * @param path A collection of interfaces defining the device.
   * @param qos The quality of service value. It could be only 0, 1 or 2.
   * @param data A collection of interfaces defining the device.
   * @return True if the device is connected to Astarte, false otherwise.
   */
  auto send_individual(std::string_view interface_name, std::string_view path, uint8_t qos,
                       const std::span<uint8_t> data) -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Disconnect the client from the Astarte MQTT broker.
   * @return an error if the disconnection operation fails.
   */
  auto disconnect() -> astarte_tl::expected<void, AstarteError>;

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
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONNECTION_H
