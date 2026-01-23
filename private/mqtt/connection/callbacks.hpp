// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_CALLBACKS_H
#define ASTARTE_MQTT_CONNECTION_CALLBACKS_H

#include <spdlog/spdlog.h>

#include <atomic>
#include <memory>
#include <string>

#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "mqtt/connection/listener.hpp"
#include "mqtt/introspection.hpp"

#include "mqtt/iaction_listener.h"
#include "mqtt/iasync_client.h"
#include "mqtt/thread_queue.h"

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
                         std::shared_ptr<std::atomic<bool>> connected,
                        std::shared_ptr<mqtt::thread_queue<mqtt::token_ptr>> connection_tokens);

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

  std::shared_ptr<mqtt::thread_queue<mqtt::token_ptr>> connection_tokens_;

  std::shared_ptr<SessionSetupListener> session_setup_listener_;
  std::shared_ptr<MqttDisconnectionListener> disconnection_listener_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONNECTION_CALLBACKS_H
