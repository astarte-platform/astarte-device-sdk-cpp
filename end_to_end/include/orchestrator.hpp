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
  explicit TestOrchestrator(const TestHttpConfig& config_http,
                            std::shared_ptr<TestDeviceFactory> factory)
      : config_http_(config_http), device_factory_(std::move(factory)) {}

  // Add test case to orchestrator
  void add_test_case(TestCase&& tc) {
    tc.add_device_factory(device_factory_);
    test_cases_.push(std::move(tc));
  }

  // Execute all test cases
  void execute_all() {
    spdlog::info("Executing all end to end test cases...");
    while (!test_cases_.empty()) {
      TestCase test_case = std::move(test_cases_.front());
      test_cases_.pop();
      test_case.execute(config_http_);
    }
  }

 private:
  std::shared_ptr<TestDeviceFactory> device_factory_;
  TestHttpConfig config_http_;
  std::queue<TestCase> test_cases_;
};
