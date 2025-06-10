// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#include <vector>

#include "case.hpp"

struct ConfigGRPC {
  std::string server_addr;
  std::string node_id;
  std::vector<std::filesystem::path> interfaces;
};

struct ConfigCURL {
  std::string appengine_url;
  std::string appengine_token;
  std::string realm;
  std::string device_id;
};

// Orchestrator for end to end tests
class TestOrchestrator {
 public:
  explicit TestOrchestrator(const struct ConfigGRPC& config_grpc,
                            const struct ConfigCURL& config_curl)
      : grpc_config_(config_grpc), curl_config_(config_curl) {}

  // Add test case to orchestrator
  void add_test_case(TestCase& test_case) {
    std::shared_ptr<AstarteDevice> device =
        std::make_shared<AstarteDevice>(grpc_config_.server_addr, grpc_config_.node_id);
    for (const std::filesystem::path& interface_path : grpc_config_.interfaces) {
      device->add_interface_from_json(interface_path);
    }

    test_case.attach_device(device);
    test_case.configure_curl(curl_config_.appengine_url, curl_config_.appengine_token,
                             curl_config_.realm, curl_config_.device_id);

    test_cases_.push_back(test_case);
  }

  // Execute all test cases
  void execute_all() {
    spdlog::info("Executing all end to end test cases...");
    for (auto& test_case : test_cases_) {
      test_case.execute();
    }
  }

 private:
  struct ConfigGRPC grpc_config_;
  struct ConfigCURL curl_config_;
  std::vector<TestCase> test_cases_;
};
