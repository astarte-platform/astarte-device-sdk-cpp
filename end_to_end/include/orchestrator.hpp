// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <queue>
#include <string>

#include "case.hpp"
#include "device_factory.hpp"

// Orchestrator for end to end tests
class TestOrchestrator {
 public:
  explicit TestOrchestrator(const struct TestHttpConfig& config_http) : config_http_(config_http) {}

  auto with_device_factory(std::shared_ptr<TestDeviceFactory> factory) -> TestOrchestrator& {
    device_factory_ = std::move(factory);
    return *this;
  }

  // Add test case to orchestrator
  void add_test_case(TestCase&& tc) { test_cases_.push(std::move(tc)); }

  // Execute a specific test case without using an Astarte device instance
  // Useful for testing pairing functionalities where the device isn't created yet.
  void execute_without_device(TestCase&& tc) {
    spdlog::info("Executing single test case (No Device Mode)");
    TestCase test_case = std::move(tc);

    // Pass nullptr for device
    test_case.execute(nullptr, config_http_);
  }

  // Execute all test cases
  void execute_all() {
    spdlog::info("Executing all end to end test cases...");

    if (!device_factory_) {
      spdlog::warn("Couldn't execute tests since no device factory has been defined.");
      return;
    }

    while (!test_cases_.empty()) {
      TestCase test_case = std::move(test_cases_.front());
      test_cases_.pop();

      // Create a fresh device instance for each test case
      std::shared_ptr<AstarteDevice> device = device_factory_->create_device();

      test_case.execute(device, config_http_);
    }
  }

 private:
  std::shared_ptr<TestDeviceFactory> device_factory_;
  struct TestHttpConfig config_http_;
  std::queue<TestCase> test_cases_;
};
