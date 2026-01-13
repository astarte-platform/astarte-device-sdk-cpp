// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <format>
#include <toml++/toml.hpp>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "constants/astarte_interfaces.hpp"
#include "orchestrator.hpp"
#include "testcases/device_add_remove_interface.hpp"
#include "testcases/device_aggregate.hpp"
#include "testcases/device_datastream.hpp"
#include "testcases/device_property.hpp"
#include "testcases/device_reconnection.hpp"
#include "testcases/device_status.hpp"
#include "testcases/mqtt/device_pairing.hpp"
#include "testcases/server_aggregate.hpp"
#include "testcases/server_datastream.hpp"
#include "testcases/server_property.hpp"

using AstarteDeviceSdk::AstarteData;
using AstarteDeviceSdk::AstarteMessage;

bool validate_config(const toml::table& config) {
  bool is_valid = true;

  // 1. Helper to check top-level string keys
  auto check_key = [&](const std::string& key) {
    if (!config[key].is_string()) {
      spdlog::error("Configuration Error: Missing or invalid string key '{}'", key);
      is_valid = false;
    }
  };

  // 2. Validate general configuration
  check_key("realm");
  check_key("device_id");
  check_key("appengine_token");
  check_key("astarte_base_url");

  // 3. Validate transport specifics
#ifdef ASTARTE_TRANSPORT_GRPC
  if (!config["grpc"].is_table()) {
    spdlog::error("Configuration Error: Missing '[grpc]' table. gRPC settings must be nested.");
    is_valid = false;
  } else {
    if (!config["grpc"]["server_addr"].is_string()) {
      spdlog::error("Configuration Error: Missing 'server_addr' inside [grpc] table.");
      is_valid = false;
    }
    if (!config["grpc"]["node_id"].is_string()) {
      spdlog::error("Configuration Error: Missing 'node_id' inside [grpc] table.");
      is_valid = false;
    }
  }
#else
  if (!config["mqtt"].is_table()) {
    spdlog::error("Configuration Error: Missing '[mqtt]' table.");
    is_valid = false;
  }
#endif

  return is_valid;
}

int main() {
  spdlog::set_level(spdlog::level::debug);

  // Parse configuration from toml
  toml::table config = toml::parse_file("end_to_end/config.toml");
  if (!validate_config(config)) {
    spdlog::error("Configuration validation failed. Aborting tests.");
    return 1;
  }

  auto realm = config["realm"].value<std::string>().value();
  auto device_id = config["device_id"].value<std::string>().value();
  auto astarte_base_url = config["astarte_base_url"].value<std::string>().value();

  struct CURLConfig curl_config = {
      .astarte_base_url = astarte_base_url,
      .appengine_token = config["appengine_token"].value<std::string>().value(),
      .realm = realm,
      .device_id = device_id};

  // Create orchestrator
  TestOrchestrator orchestrator(curl_config);

  // Create configuration structs
#ifdef ASTARTE_TRANSPORT_GRPC
  TestGrpcDeviceConfig grpc_conf = {
      .server_addr = config["grpc"]["server_addr"].value<std::string>().value(),
      .node_id = config["grpc"]["node_id"].value<std::string>().value(),
      .interfaces = {
          astarte_interfaces::DeviceDatastream::FILE,
          astarte_interfaces::ServerDatastream::FILE,
          astarte_interfaces::DeviceAggregate::FILE,
          astarte_interfaces::ServerAggregate::FILE,
          astarte_interfaces::DeviceProperty::FILE,
          astarte_interfaces::ServerProperty::FILE,
      }};
  orchestrator.with_device_factory(std::make_shared<TestGrpcDeviceFactory>(grpc_conf));

  orchestrator.add_test_case(testcases::device_status());
  orchestrator.add_test_case(testcases::device_reconnection());
  orchestrator.add_test_case(testcases::device_add_remove_interface());
  orchestrator.add_test_case(testcases::device_datastream());
  orchestrator.add_test_case(testcases::server_datastream());
  orchestrator.add_test_case(testcases::device_aggregate());
  orchestrator.add_test_case(testcases::server_aggregate());
  orchestrator.add_test_case(testcases::device_property());
  orchestrator.add_test_case(testcases::device_property_getter());
  orchestrator.add_test_case(testcases::server_property());
  orchestrator.add_test_case(testcases::server_property_on_new_device());

  // Execute all test cases
  orchestrator.execute_all();
#else   // ASTARTE_TRANSPORT_GRPC
  auto pairing_token_opt = config.at("mqtt").at_path("pairing_token").value<std::string>();
  auto credential_secret_opt = config.at("mqtt").at_path("credential_secret").value<std::string>();

  if (pairing_token_opt) {
    orchestrator.execute_without_device(testcases::device_pairing(pairing_token_opt.value()));
  } else if (credential_secret_opt) {
    auto store_dir = config["mqtt"]["store_dir"].value<std::string>().value();

    TestMqttDeviceConfig mqtt_conf = {
        .realm = realm,
        .device_id = device_id,
        .credential_secret = credential_secret_opt.value(),
        .pairing_url = astarte_fmt::format("{}/pairing", astarte_base_url),
        .store_dir = store_dir,
        .interfaces = {
            astarte_interfaces::DeviceDatastream::FILE,
            astarte_interfaces::ServerDatastream::FILE,
            astarte_interfaces::DeviceAggregate::FILE,
            astarte_interfaces::ServerAggregate::FILE,
            astarte_interfaces::DeviceProperty::FILE,
            astarte_interfaces::ServerProperty::FILE,
        }};
    orchestrator.with_device_factory(std::make_shared<TestMqttDeviceFactory>(std::move(mqtt_conf)));

    orchestrator.add_test_case(testcases::device_status());
    // TODO: add test cases to execute here

    // Execute all test cases
    orchestrator.execute_all();
  } else {
    spdlog::error("At least one between credentials secret or pairing token mut be provided");
    return 1;
  }
#endif  // ASTARTE_TRANSPORT_GRPC

  return 0;
}
