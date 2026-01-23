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

namespace AstarteDeviceSdk {

class SessionSetupListener : public virtual mqtt::iaction_listener {
 public:
  explicit SessionSetupListener(
    std::shared_ptr<mqtt::thread_queue<mqtt::token_ptr>> connection_tokens,
    std::shared_ptr<std::atomic<bool>> connected);

 private:
  void on_failure(const mqtt::token& tok) override;
  void on_success(const mqtt::token& tok) override;

  std::shared_ptr<mqtt::thread_queue<mqtt::token_ptr>> connection_tokens_;
  std::shared_ptr<std::atomic<bool>> connected_;

};

class MqttDisconnectionListener : public virtual mqtt::iaction_listener {
 public:
  explicit MqttDisconnectionListener(std::shared_ptr<std::atomic<bool>> connected);

 private:
  void on_failure(const mqtt::token& tok) override;
  void on_success(const mqtt::token& tok) override;

  std::shared_ptr<std::atomic<bool>> connected_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONNECTION_H
