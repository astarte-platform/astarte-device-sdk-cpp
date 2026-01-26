// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_LISTENER_H
#define ASTARTE_MQTT_CONNECTION_LISTENER_H

#include <spdlog/spdlog.h>

#include <atomic>
#include <memory>

#include "mqtt/iaction_listener.h"
#include "mqtt/thread_queue.h"
#include "mqtt/token.h"

namespace AstarteDeviceSdk::mqtt_connection {

/**
 * @brief Listener for MQTT actions performed during the Astarte session setup.
 *
 * This class implements `mqtt::iaction_listener` to handle the completion (success or failure)
 * of asynchronous operations—such as subscriptions and introspection publishing—that occur
 * when establishing a session with Astarte.
 */
class SessionSetupListener : public virtual mqtt::iaction_listener {
 public:
  /**
   * @brief Construct a new Session Setup Listener object.
   *
   * @param session_setup_tokens Queue where tokens from pending session setup actions are stored.
   * @param connected A flag stating if the client is correctly connected to Astarte.
   */
  explicit SessionSetupListener(
      std::shared_ptr<mqtt::thread_queue<mqtt::token_ptr>> session_setup_tokens,
      std::shared_ptr<std::atomic<bool>> connected);

 private:
  /**
   * @brief Called when an asynchronous action fails.
   * @param tok The token associated with the failed action.
   */
  void on_failure(const mqtt::token& tok) override;

  /**
   * @brief Called when an asynchronous action completes successfully.
   * @param tok The token associated with the successful action.
   */
  void on_success(const mqtt::token& tok) override;

  /// @brief Queue to store the tokens of the session setup actions.
  std::shared_ptr<mqtt::thread_queue<mqtt::token_ptr>> session_setup_tokens_;
  /// @brief The flag stating if the device is successfully connected to Astarte.
  std::shared_ptr<std::atomic<bool>> connected_;
};

/**
 * @brief Listener for the MQTT disconnection action.
 *
 * This class implements `mqtt::iaction_listener` to handle the callbacks specifically
 * related to the explicit disconnection of the client.
 */
class DisconnectionListener : public virtual mqtt::iaction_listener {
 public:
  /**
   * @brief Construct a new Disconnection Listener object.
   *
   * @param connected A flag stating if the client is correctly connected to Astarte.
   */
  explicit DisconnectionListener(std::shared_ptr<std::atomic<bool>> connected);

 private:
  /**
   * @brief Called when the disconnection action fails.
   * @param tok The token associated with the failed action.
   */
  void on_failure(const mqtt::token& tok) override;

  /**
   * @brief Called when the disconnection action completes successfully.
   * @param tok The token associated with the successful action.
   */
  void on_success(const mqtt::token& tok) override;

  /// @brief The flag stating if the device is successfully connected to Astarte.
  std::shared_ptr<std::atomic<bool>> connected_;
};

}  // namespace AstarteDeviceSdk::mqtt_connection

#endif  // ASTARTE_MQTT_CONNECTION_H
