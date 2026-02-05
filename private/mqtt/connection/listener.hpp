// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_LISTENER_H
#define ASTARTE_MQTT_CONNECTION_LISTENER_H

/**
 * @file private/mqtt/connection/listener.hpp
 * @brief MQTT Action Listeners.
 *
 * @details This file defines listener classes implementing `paho_mqtt::iaction_listener`.
 * These listeners track the success or failure of asynchronous MQTT operations like
 * session setup (connect, subscribe) and disconnection.
 */

#include <spdlog/spdlog.h>

#include <atomic>
#include <memory>

#include "mqtt/iaction_listener.h"
#include "mqtt/thread_queue.h"
#include "mqtt/token.h"

namespace astarte::device::mqtt::connection {

namespace paho_mqtt = ::mqtt;

/**
 * @brief Listener for MQTT actions performed during the Astarte session setup.
 *
 * @details This class implements `paho_mqtt::iaction_listener` to handle the completion (success or
 * failure) of asynchronous operations—such as subscriptions and introspection publishing—that occur
 * when establishing a session with Astarte. It ensures tokens are tracked until completion.
 */
class SessionSetupListener : public virtual paho_mqtt::iaction_listener {
 public:
  /**
   * @brief Constructs a new Session Setup Listener object.
   *
   * @param[in] session_setup_tokens Queue where tokens from pending session setup actions are
   * stored.
   * @param[in] connected A flag stating if the client is correctly connected to Astarte.
   */
  explicit SessionSetupListener(
      std::shared_ptr<paho_mqtt::thread_queue<paho_mqtt::token_ptr>> session_setup_tokens,
      std::shared_ptr<std::atomic<bool>> connected);

 private:
  /**
   * @brief Called when an asynchronous action fails.
   * @param[in] tok The token associated with the failed action.
   */
  void on_failure(const paho_mqtt::token& tok) override;

  /**
   * @brief Called when an asynchronous action completes successfully.
   * @param[in] tok The token associated with the successful action.
   */
  void on_success(const paho_mqtt::token& tok) override;

  /// @brief Queue to store the tokens of the session setup actions.
  std::shared_ptr<paho_mqtt::thread_queue<paho_mqtt::token_ptr>> session_setup_tokens_;
  /// @brief The flag stating if the device is successfully connected to Astarte.
  std::shared_ptr<std::atomic<bool>> connected_;
};

/**
 * @brief Listener for the MQTT disconnection action.
 *
 * @details This class implements `paho_mqtt::iaction_listener` to handle the callbacks specifically
 * related to the explicit disconnection of the client, ensuring the connection flag is updated
 * appropriately.
 */
class DisconnectionListener : public virtual paho_mqtt::iaction_listener {
 public:
  /**
   * @brief Constructs a new Disconnection Listener object.
   *
   * @param[in] connected A flag stating if the client is correctly connected to Astarte.
   */
  explicit DisconnectionListener(std::shared_ptr<std::atomic<bool>> connected);

 private:
  /**
   * @brief Called when the disconnection action fails.
   * @param[in] tok The token associated with the failed action.
   */
  void on_failure(const paho_mqtt::token& tok) override;

  /**
   * @brief Called when the disconnection action completes successfully.
   * @param[in] tok The token associated with the successful action.
   */
  void on_success(const paho_mqtt::token& tok) override;

  /// @brief The flag stating if the device is successfully connected to Astarte.
  std::shared_ptr<std::atomic<bool>> connected_;
};

}  // namespace astarte::device::mqtt::connection

#endif  // ASTARTE_MQTT_CONNECTION_H
