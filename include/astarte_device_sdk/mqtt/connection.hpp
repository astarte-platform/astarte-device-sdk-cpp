// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_H
#define ASTARTE_MQTT_CONNECTION_H

#include <spdlog/spdlog.h>

#include <format>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "astarte_device_sdk/mqtt/introspection.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "mqtt/async_client.h"
#include "mqtt/iasync_client.h"

namespace AstarteDeviceSdk {

// Forward declaration to allow usage in the ActionListener
class ConnectionCallback;

/**
 * @brief Implements the MQTT `iaction_listener` to handle the result of the initial connection
 * attempt.
 *
 * This class acts as a trigger. When the asynchronous connect operation succeeds,
 * it delegates the actual session setup (subscriptions, introspection) to the `ConnectionCallback`.
 */
class ConnectionActionListener : public virtual mqtt::iaction_listener {
  /// @brief Reference to the main callback handler that contains the setup logic.
  ConnectionCallback& callback_handler_;

  /**
   * @brief Called when the connection attempt fails.
   * @param tok The token associated with the failed action.
   */
  void on_failure(const mqtt::token& tok) override;

  /**
   * @brief Called when the connection attempt succeeds.
   *
   * Checks if a session is present. If not, it triggers the setup routine
   * in the `ConnectionCallback`.
   *
   * @param tok The token associated with the successful action.
   */
  void on_success(const mqtt::token& tok) override;

 public:
  /**
   * @brief Construct a new Connection Action Listener.
   *
   * @param callback_handler Reference to the initialized ConnectionCallback object.
   */
  explicit ConnectionActionListener(ConnectionCallback& callback_handler);
};

/**
 * @brief Implements `mqtt::callback` to handle connection life-cycle events and session setup.
 *
 * This class owns the device state (ID, introspection) and is responsible for
 * subscribing to topics and publishing introspection when a session is established.
 */
class ConnectionCallback : public virtual mqtt::callback {
 public:
  /**
   * @brief Construct a new Connection Callback object.
   *
   * @param client Pointer to the MQTT asynchronous client.
   * @param device_id The Astarte Device ID.
   * @param introspection A reference to the vector of device interfaces.
   */
  ConnectionCallback(mqtt::iasync_client* client, std::string device_id,
                     std::vector<Interface>& introspection);

  /**
   * @brief Performs the Astarte session setup.
   *
   * This includes subscribing to control/data topics and sending the introspection
   * and emptyCache messages.
   *
   * @param session_present Indicates if the broker resumed a previous session.
   * If true, subscriptions might be skipped depending on logic.
   */
  void perform_session_setup(bool session_present);

  /**
   * @brief Construct a new Connection Callback object.
   *
   * @param client Pointer to the MQTT asynchronous client.
   * @param realm The Astarte realm name.
   * @param device_id The Astarte Device ID.
   * @param introspection A reference to the collection of device interfaces.
   */
  ConnectionCallback(mqtt::iasync_client* client, std::string realm, std::string device_id,
                     Introspection& introspection);

 private:
  /**
   * @brief Subscribes the client to all required Astarte topics.
   *
   * Includes the control topic and all topics for server-owned interfaces.
   */
  void setup_subscriptions();

  /**
   * @brief Publishes the device's introspection to Astarte.
   */
  void send_introspection();

  /**
   * @brief Sends an "emptyCache" message to Astarte.
   */
  void send_emptycache();

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
  Introspection& introspection_;
};

/**
 * @brief Manages the MQTT connection to an Astarte instance.
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
   * Initializes connection parameters, performs pairing if necessary,
   * and configures the MQTT client.
   *
   * @param cfg The MQTT configuration object containing connection details.
   * @return The MQTT connection object, an error otherwise.
   */
  static auto create(MqttConfig cfg) -> astarte_tl::expected<MqttConnection, AstarteError>;

  /**
   * @brief Connects the client to the Astarte MQTT broker.
   * @param introspection A collection of interfaces defining the device.
   * @return an error if the connection operation fails.
   */
  auto connect(Introspection& introspection) -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Disconnect the client from the Astarte MQTT broker.
   * @return an error if the disconnection operation fails.
   */
  auto disconnect() -> astarte_tl::expected<void, AstarteError>;

 private:
  MqttConnection(MqttConfig cfg, mqtt::connect_options options,
                 std::unique_ptr<mqtt::async_client> client);

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
