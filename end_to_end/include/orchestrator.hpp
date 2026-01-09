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

#if defined(ASTARTE_TRANSPORT_GRPC)
#include "astarte_device_sdk/grpc/device_grpc.hpp"
#else
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#endif
#include "case.hpp"
#include "transport.hpp"

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

  // set a specific transport for the orchestrator
  auto with_transport_config(TransportConfigVariant transport_config) -> TestOrchestrator& {
    transport_config_ = std::optional(std::move(transport_config));
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
    if (!transport_config_) {
      spdlog::warn("Couldn't execute tests since no transport has been defined.");
      return;
    }

    // Execute each test case sequentially cleaning the test cases queue in the process
    for (; !test_cases_.empty(); test_cases_.pop()) {
      TestCase test_case = std::move(test_cases_.front());

      test_case.configure_curl(curl_config_.astarte_base_url, curl_config_.appengine_token,
                               curl_config_.realm, curl_config_.device_id);

      // Create a new device of the correct transport type
#if defined(ASTARTE_TRANSPORT_GRPC)
      auto config_grpc = std::get<struct GrpcTestConfig>(transport_config_.value());

      std::shared_ptr<AstarteDeviceGrpc> device_grpc =
          std::make_shared<AstarteDeviceGrpc>(config_grpc.server_addr, config_grpc.node_id);

      for (const std::filesystem::path& interface_path : config_grpc.interfaces) {
        auto res = device_grpc->add_interface_from_file(interface_path);
        if (!res) {
          throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
        }
      }
      test_case.attach_device(device_grpc);
#else
      auto config_mqtt = std::get<MqttTestConfig>(*std::move(transport_config_));

      auto res = AstarteDeviceMqtt::create(std::move(config_mqtt.cfg));
      if (!res) {
        spdlog::error("Couldn't create an Astarte MQTT device.");
        throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
      }
      std::shared_ptr<AstarteDeviceMqtt> device_mqtt =
          std::make_shared<AstarteDeviceMqtt>(*std::move(res));

      for (const std::filesystem::path& interface_path : config_mqtt.interfaces) {
        auto res = device_mqtt->add_interface_from_file(interface_path);
        if (!res) {
          spdlog::error("Couldn't add interface.");
          throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
        }
      }

      test_case.attach_device(device_mqtt);
#endif

      // run the isolated test
      test_case.start();
      test_case.execute();
    }
  }

 private:
  std::optional<TransportConfigVariant> transport_config_;
  struct CURLConfig curl_config_;
  std::queue<TestCase> test_cases_;
};
