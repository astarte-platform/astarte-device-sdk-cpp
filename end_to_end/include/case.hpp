// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "action.hpp"
#include "astarte_device_sdk/device.hpp"
#include "device_factory.hpp"

using AstarteDeviceSdk::AstarteDevice;

// End to end test case
class TestCase {
 public:
  TestCase(std::string name, std::vector<Action> actions, std::string device_id,
           bool generate_device = true)
      : name_(std::move(name)),
        actions_(std::move(actions)),
        device_id_(std::move(device_id)),
        generate_device_(generate_device) {}

  ~TestCase() = default;

  // TestCase is movable but not copyable
  TestCase(const TestCase&) = delete;
  TestCase(TestCase&&) = default;
  auto operator=(const TestCase&) -> TestCase& = delete;
  auto operator=(TestCase&& other) -> TestCase& = default;

  void add_device_factory(std::shared_ptr<TestDeviceFactory> factory) {
    device_factory_ = std::move(factory);
  }

  // Executes the test case with the provided HTTP configuration.
  void execute(const TestHttpConfig& http_config) {
    spdlog::info("Starting Test Case: {}", name_);

    // 1. Create a fresh device instance for each test case
    std::shared_ptr<AstarteDevice> device = nullptr;
    if (generate_device_) {
      if (!device_factory_) {
        spdlog::error("Couldn't execute test case since no device factory has been defined.");
        return;
      }
      device = device_factory_->create_device();
    }

    // 2. Create the RX Queue for this specific run
    auto rx_queue = std::make_shared<SharedQueue<AstarteMessage>>();

    // 3. Start the background reception thread (if device is present)
    std::unique_ptr<std::jthread> rx_thread;

    if (device) {
      rx_thread = std::make_unique<std::jthread>([&, dev = device](std::stop_token token) {
        while (!token.stop_requested()) {
          auto incoming = dev->poll_incoming(std::chrono::milliseconds(100));
          if (incoming.has_value()) {
            AstarteMessage msg(incoming.value());
            spdlog::debug("Handler received message: {}", msg.get_path());
            rx_queue->push(msg);
          }
        }
      });
    }

    // 4. Build the context
    TestCaseContext ctx{
        .device_id = device_id_, .device = device, .rx_queue = rx_queue, .http = http_config};

    // 5. Execute all actions
    try {
      for (const auto& action : actions_) {
        action(ctx);
      }
      spdlog::info("Test Case '{}' passed.", name_);
    } catch (const std::exception& e) {
      spdlog::error("Test Case '{}' failed: {}", name_, e.what());
      throw;  // Re-throw to be caught by orchestrator or main
    }
  }

 private:
  std::string name_;
  std::string device_id_;
  bool generate_device_;
  std::shared_ptr<TestDeviceFactory> device_factory_;
  std::vector<Action> actions_;
};
