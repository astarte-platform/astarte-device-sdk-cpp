// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/connection/listener.hpp"

namespace AstarteDeviceSdk {

SessionSetupListener::SessionSetupListener(
    std::shared_ptr<mqtt::thread_queue<mqtt::token_ptr>> connection_tokens,
    std::shared_ptr<std::atomic<bool>> connected)
    : connection_tokens_(connection_tokens), connected_(connected) {}

void SessionSetupListener::on_failure(const mqtt::token& tok) {
  spdlog::warn("Operation failed. {}", tok.get_message_id());
}

void SessionSetupListener::on_success(const mqtt::token& tok) {
  spdlog::info("Operation was a success. {}", tok.get_message_id());
  connection_tokens_->get();
  if (connection_tokens_->empty()) {
    spdlog::info("Astarte handshake completed.");
    connected_->store(true);
  }
}

MqttDisconnectionListener::MqttDisconnectionListener(std::shared_ptr<std::atomic<bool>> connected)
    : connected_(connected) {}

void MqttDisconnectionListener::on_failure(const mqtt::token& tok) {
  spdlog::error("MQTT disconnection failed.");
}

void MqttDisconnectionListener::on_success(const mqtt::token& tok) {
  spdlog::info("MQTT disconnection success.");
  connected_->store(false);
}

}  // namespace AstarteDeviceSdk
