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

using AstarteDeviceSdk::AstarteDevice;

// End to end test case
class TestCase {
 public:
  TestCase(std::string name, std::vector<Action> actions)
      : name_(std::move(name)), actions_(std::move(actions)) {}

  ~TestCase() = default;

  // TestCase is movable but not copyable
  TestCase(const TestCase&) = delete;
  TestCase(TestCase&&) = default;
  auto operator=(const TestCase&) -> TestCase& = delete;
  auto operator=(TestCase&& other) -> TestCase& = default;

  /**
   * @brief Executes the test case with the provided device and HTTP configuration.
   *
   * @param device The Astarte device instance to use. Can be nullptr if a device is not required.
   * @param http_config Configuration for REST API interactions.
   */
  void execute(std::shared_ptr<AstarteDevice> device, const TestHttpConfig& http_config) {
    spdlog::info("Starting Test Case: {}", name_);

    // 1. Create the RX Queue for this specific run
    auto rx_queue = std::make_shared<SharedQueue<AstarteMessage>>();

    // 2. Start the background reception thread (if device is present)
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

    // 3. Build the context
    TestCaseContext ctx{.device = device, .rx_queue = rx_queue, .http = http_config};

    // 4. Execute all actions
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
  std::vector<Action> actions_;
};
