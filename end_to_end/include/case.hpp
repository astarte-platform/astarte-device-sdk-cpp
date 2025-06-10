// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <thread>
#include <variant>
#include <vector>

#include "action.hpp"
#include "astarte_device_sdk/device.hpp"

using AstarteDeviceSdk::AstarteDevice;

// End to end test case
class TestCase {
 public:
  explicit TestCase(const std::string& name,
                    const std::vector<std::shared_ptr<TestAction>>& actions)
      : name_(name),
        actions_(actions),
        kill_reception_(std::make_shared<std::atomic_bool>(false)),
        rx_queue_(std::make_shared<SharedQueue<AstarteMessage>>()) {}
  ~TestCase() {
    if (thread_ && thread_->joinable()) {
      *kill_reception_ = true;
      thread_->join();
    }
  }

  void configure_curl(const std::string& appengine_url, const std::string& appengine_token,
                      const std::string& realm, const std::string& device_id) {
    for (const auto& action : actions_) {
      action->configure_curl(appengine_url, appengine_token, realm, device_id);
    }
  }
  void attach_device(const std::shared_ptr<AstarteDevice>& device) {
    for (const auto& action : actions_) {
      action->attach_device(device, rx_queue_, kill_reception_);
    }
    device_ = device;
  }
  void start() {
    if (!thread_) {
      thread_ = std::make_shared<std::thread>(&TestCase::reception_handler, this);
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
  std::shared_ptr<std::atomic_bool> kill_reception_;
  std::shared_ptr<SharedQueue<AstarteMessage>> rx_queue_;
  std::shared_ptr<std::thread> thread_;

  void reception_handler() {
    while (!(*kill_reception_)) {
      auto incoming = device_->poll_incoming();
      if (incoming.has_value()) {
        AstarteMessage msg(incoming.value());
        spdlog::debug("Handler received message: {}", msg.format());
        rx_queue_->push(msg);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
};
