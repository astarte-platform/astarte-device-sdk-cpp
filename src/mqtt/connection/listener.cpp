// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/connection/listener.hpp"

#include <utility>

namespace AstarteDeviceSdk::mqtt_connection {

SessionSetupListener::SessionSetupListener(
    std::shared_ptr<mqtt::thread_queue<mqtt::token_ptr>> session_setup_tokens,
    std::shared_ptr<std::atomic<bool>> connected)
    : session_setup_tokens_(std::move(session_setup_tokens)), connected_(std::move(connected)) {}

void SessionSetupListener::on_failure(const mqtt::token& tok) {
  spdlog::warn("Operation failed. {}", tok.get_message_id());
}

void SessionSetupListener::on_success(const mqtt::token& tok) {
  spdlog::info("Operation was a success. {}", tok.get_message_id());
  session_setup_tokens_->get();
  if (session_setup_tokens_->empty()) {
    spdlog::info("Astarte handshake completed.");
    connected_->store(true);
  }
}

DisconnectionListener::DisconnectionListener(std::shared_ptr<std::atomic<bool>> connected)
    : connected_(std::move(connected)) {}

void DisconnectionListener::on_failure(const mqtt::token& /*tok*/) {
  spdlog::error("MQTT disconnection failed.");
}

void DisconnectionListener::on_success(const mqtt::token& /*tok*/) {
  spdlog::info("MQTT disconnection success.");
  connected_->store(false);
}

}  // namespace AstarteDeviceSdk::mqtt_connection
