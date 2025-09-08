// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#include <chrono>
#include <functional>
#include <variant>
#include <vector>

#include "case.hpp"
#include "transport.hpp"

using namespace std::chrono_literals;

struct CURLConfig {
  std::string appengine_url;
  std::string appengine_token;
  std::string realm;
  std::string device_id;
};

// Orchestrator for end to end tests
class TestOrchestrator {
 public:
  explicit TestOrchestrator(const TransportConfigVariant transport_config,
                            const struct CURLConfig& config_curl)
      : transport_config_(transport_config), curl_config_(config_curl) {}

  // Add test case to orchestrator
  void add_test_case(TestCase&& tc) { test_cases_.push(std::move(tc)); }

  // Execute all test cases
  void execute_all() {
    spdlog::info("Executing all end to end test cases...");

    // Execute each test case sequentially cleaning the test cases queue in the process
    for (; !test_cases_.empty(); test_cases_.pop()) {
      TestCase test_case = std::move(test_cases_.front());

// Create a new device of the correct transport type
#ifdef ASTARTE_TRANSPORT_GRPC
      auto config_grpc = std::get<struct GRPCConfig>(transport_config_);

      std::shared_ptr<AstarteDeviceGRPC> device_grpc =
          std::make_shared<AstarteDeviceGRPC>(config_grpc.server_addr, config_grpc.node_id);

      for (const std::filesystem::path& interface_path : config_grpc.interfaces) {
        device_grpc->add_interface_from_file(interface_path);
      }
      test_case.attach_device(device_grpc);
#else
      auto config_mqtt = std::get<struct MQTTConfig>(transport_config_);

      std::shared_ptr<AstarteDeviceMQTT> device_mqtt =
          std::make_shared<AstarteDeviceMQTT>(config_mqtt.cfg);

      // TODO: decomment once the add_interface functionality has been implemented
      //   for (const std::filesystem::path& interface_path : config_mqtt.interfaces) {
      // device_mqtt->add_interface_from_file(interface_path);
      // }

      test_case.attach_device(device_mqtt);
#endif

      test_case.configure_curl(curl_config_.appengine_url, curl_config_.appengine_token,
                               curl_config_.realm, curl_config_.device_id);

      // run the isolated test
      test_case.start();
      test_case.execute();
    }
  }

 private:
  TransportConfigVariant transport_config_;
  struct CURLConfig curl_config_;
  std::queue<TestCase> test_cases_;
};
