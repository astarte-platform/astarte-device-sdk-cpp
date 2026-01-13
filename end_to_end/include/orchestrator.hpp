// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#include <chrono>
#include <functional>
#include <optional>
#include <variant>
#include <vector>

#include "case.hpp"
#include "device_factory.hpp"

using namespace std::chrono_literals;

struct CURLConfig {
  std::string astarte_base_url;
  std::string appengine_token;
  std::string realm;
  std::string device_id;
};

// Orchestrator for end to end tests
class TestOrchestrator {
 public:
  explicit TestOrchestrator(const struct CURLConfig& config_curl) : curl_config_(config_curl) {}

  auto with_device_factory(std::shared_ptr<TestDeviceFactory> factory) -> TestOrchestrator& {
    device_factory_ = std::move(factory);
    return *this;
  }

  // Add test case to orchestrator
  void add_test_case(TestCase&& tc) { test_cases_.push(std::move(tc)); }

  // Execute a specific test case without using an Astarte device
  //
  // It could be useful to test pairing functionalities.
  void execute_without_device(TestCase&& tc) {
    spdlog::info("Executing single test case");
    TestCase test_case = std::move(tc);

    test_case.configure_curl(curl_config_.astarte_base_url, curl_config_.appengine_token,
                             curl_config_.realm, curl_config_.device_id);

    test_case.execute();
  }

  // Execute all test cases
  void execute_all() {
    spdlog::info("Executing all end to end test cases...");

    // if no transport has been defined, terminate the execution
    if (!device_factory_) {
      spdlog::warn("Couldn't execute tests since no device factory has been defined.");
      return;
    }

    // Execute each test case sequentially cleaning the test cases queue in the process
    for (; !test_cases_.empty(); test_cases_.pop()) {
      TestCase test_case = std::move(test_cases_.front());

      test_case.configure_curl(curl_config_.astarte_base_url, curl_config_.appengine_token,
                               curl_config_.realm, curl_config_.device_id);

      std::shared_ptr<AstarteDevice> device = device_factory_->create_device();
      test_case.attach_device(device);

      test_case.start();
      test_case.execute();
    }
  }

 private:
  std::shared_ptr<TestDeviceFactory> device_factory_;
  struct CURLConfig curl_config_;
  std::queue<TestCase> test_cases_;
};
