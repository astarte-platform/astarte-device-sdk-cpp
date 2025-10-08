// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "action.hpp"

#ifndef ASTARTE_TRANSPORT_GRPC
#include "action_mqtt.hpp"
#endif  // ASTARTE_TRANSPORT_GRPC

#include "astarte_device_sdk/device.hpp"

using AstarteDeviceSdk::AstarteDevice;

// End to end test case
class TestCase {
 public:
  explicit TestCase(const std::string& name,
                    const std::vector<std::shared_ptr<TestAction>>& actions)
      : name_(name),
        actions_(actions),
        rx_queue_(std::make_shared<SharedQueue<AstarteMessage>>()) {}

  ~TestCase() = default;

  // TestCase is not copyable
  TestCase(const TestCase&) = delete;
  TestCase(TestCase&&) = default;
  auto operator=(const TestCase&) -> TestCase& = delete;
  auto operator=(TestCase&& other) -> TestCase& = default;
  // TestCase is movable

  void configure_curl(const std::string& astarte_base_url, const std::string& appengine_token,
                      const std::string& realm, const std::string& device_id) {
    for (const auto& action : actions_) {
      action->configure_curl(astarte_base_url, appengine_token, realm, device_id);
    }
  }

  void attach_device(const std::shared_ptr<AstarteDevice>& device) {
    for (const auto& action : actions_) {
      action->attach_device(device, rx_queue_);
    }
    device_ = device;
  }

  void start() {
    if (!thread_) {
      thread_ = std::make_unique<std::jthread>(
          [this](std::stop_token stoken) { this->reception_handler(stoken); });
    }
  }

  void execute() {
    spdlog::info("Executing test case: {}.", name_);
    for (const auto& action : actions_) {
      action->execute(name_);
    }
  }

 private:
  std::vector<std::shared_ptr<TestAction>> actions_;
  std::string name_;
  std::shared_ptr<AstarteDevice> device_;
  std::shared_ptr<SharedQueue<AstarteMessage>> rx_queue_;
  std::unique_ptr<std::jthread> thread_;

  void reception_handler(std::stop_token token) {
    while (!token.stop_requested()) {
      auto incoming = device_->poll_incoming(std::chrono::milliseconds(100));
      if (incoming.has_value()) {
        AstarteMessage msg(incoming.value());
        spdlog::debug("Handler received message: {}", msg);
        rx_queue_->push(msg);
      }
    }
  }
};
