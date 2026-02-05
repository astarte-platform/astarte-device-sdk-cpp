// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_CALLBACKS_H
#define ASTARTE_MQTT_CONNECTION_CALLBACKS_H

/**
 * @file private/mqtt/connection/callbacks.hpp
 * @brief MQTT Client Callback implementation.
 *
 * @details This file defines the `Callback` class, which implements the `paho_mqtt::callback`
 * interface. It handles asynchronous events from the MQTT broker, such as connection success,
 * disconnection, message arrival, and delivery confirmation, as well as managing the Astarte
 * session setup sequence.
 */

#include <spdlog/spdlog.h>

#include <atomic>
#include <memory>
#include <string>

#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "mqtt/connection/listener.hpp"
#include "mqtt/iaction_listener.h"
#include "mqtt/iasync_client.h"
#include "mqtt/introspection.hpp"
#include "mqtt/thread_queue.h"

namespace astarte::device::mqtt::connection {

namespace paho_mqtt = ::mqtt;

/**
 * @brief Implements `paho_mqtt::callback` to handle connection life-cycle events and session setup.
 *
 * @details This class acts as the central event handler for the underlying Paho MQTT client.
 * It owns the device state (Realm, ID, Introspection) and is responsible for the critical
 * "Session Setup" phase: subscribing to control topics and server-owned interfaces, and
 * publishing the device introspection.
 */
class Callback : public virtual paho_mqtt::callback {
 public:
  /**
   * @brief Constructs a new Connection Callback object.
   *
   * @param[in] client Pointer to the MQTT asynchronous client instance.
   * @param[in] realm The Astarte realm name.
   * @param[in] device_id The Astarte Device ID.
   * @param[in] introspection A reference to the collection of device interfaces.
   * @param[in] connected A flag shared with the main connection object to track connectivity
   * status.
   * @param[in] session_setup_tokens Queue for storing tokens related to session setup actions
   * (subscriptions, publications) to ensure they complete before declaring the device ready.
   */
  Callback(
      paho_mqtt::iasync_client* client, std::string realm, std::string device_id,
      std::shared_ptr<Introspection> introspection,
      const std::shared_ptr<std::atomic<bool>>& connected,
      const std::shared_ptr<paho_mqtt::thread_queue<paho_mqtt::token_ptr>>& session_setup_tokens);

  /**
   * @brief Performs the Astarte session setup sequence.
   *
   * @details This method triggers the necessary steps to initialize an Astarte session:
   * 1. Subscribes to the device control topic and server-owned interface topics.
   * 2. Publishes the device introspection.
   * 3. Publishes the `emptyCache` message if required.
   *
   * @param[in] session_present Indicates if the broker resumed a previous persistent session.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto perform_session_setup(bool session_present) -> astarte_tl::expected<void, Error>;

 private:
  /**
   * @brief Subscribes the client to all required Astarte topics.
   *
   * @details Subscriptions include the device's control topic (for housekeeping) and
   * the topics corresponding to any server-owned interfaces defined in the introspection.
   *
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto setup_subscriptions() -> astarte_tl::expected<void, Error>;

  /**
   * @brief Publishes the device's introspection to Astarte.
   *
   * @details Sends the list of supported interfaces and versions to the introspection topic.
   *
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto send_introspection() -> astarte_tl::expected<void, Error>;

  /**
   * @brief Sends an "emptyCache" message to Astarte.
   *
   * @details This signals the platform to invalidate any stale cache for this device.
   *
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto send_emptycache() -> astarte_tl::expected<void, Error>;

  /**
   * @brief Called by the client when the connection is established.
   * @details This override handles automatic reconnections by the Paho library.
   *
   * @param[in] cause The cause of the connection (e.g., "automatic reconnect").
   */
  void connected(const std::string& cause) override;

  /**
   * @brief Called when the connection is lost.
   * @details Updates the internal connected flag and logs the reason.
   *
   * @param[in] cause The reason for the disconnection.
   */
  void connection_lost(const std::string& cause) override;

  /**
   * @brief Called when a message arrives from the broker.
   * @details Processes incoming messages, routing them to the appropriate handler within the SDK.
   *
   * @param[in] msg The received message.
   */
  void message_arrived(paho_mqtt::const_message_ptr msg) override;

  /**
   * @brief Called when a message delivery is complete.
   * @details Used for QoS 1 and 2 messages to confirm receipt by the broker.
   *
   * @param[in] token The delivery token associated with the message.
   */
  void delivery_complete(paho_mqtt::delivery_token_ptr token) override;

  /// @brief Pointer to the MQTT client, used for operations like subscribe.
  paho_mqtt::iasync_client* client_;
  /// @brief The Astarte Realm name.
  std::string realm_;
  /// @brief The Astarte Device ID.
  std::string device_id_;
  /// @brief Reference to the device's introspection.
  std::shared_ptr<Introspection> introspection_;
  /// @brief The flag stating if the device is successfully connected to Astarte.
  std::shared_ptr<std::atomic<bool>> connected_;
  /// @brief Paho MQTT tokens for the messages of an Astarte session setup.
  std::shared_ptr<paho_mqtt::thread_queue<paho_mqtt::token_ptr>> session_setup_tokens_;
  /// @brief Paho MQTT listener for the messages of an Astarte session setup.
  std::shared_ptr<SessionSetupListener> session_setup_listener_;
  /// @brief Paho MQTT listener for the disconnection.
  std::shared_ptr<DisconnectionListener> disconnection_listener_;
};

}  // namespace astarte::device::mqtt::connection

#endif  // ASTARTE_MQTT_CONNECTION_CALLBACKS_H
