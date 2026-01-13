// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include <format>
#include <memory>
#include <string>
#include <toml++/toml.hpp>

#include "orchestrator.hpp"

// Test Case Headers
#include "testcases/device_add_remove_interface.hpp"
#include "testcases/device_aggregate.hpp"
#include "testcases/device_datastream.hpp"
#include "testcases/device_property.hpp"
#include "testcases/device_reconnection.hpp"
#include "testcases/device_status.hpp"
#include "testcases/server_aggregate.hpp"
#include "testcases/server_datastream.hpp"
#include "testcases/server_property.hpp"

// Pairing is only available for MQTT
#ifndef ASTARTE_TRANSPORT_GRPC
#include "testcases/mqtt/device_pairing.hpp"
#endif

// Constants
#include "constants/astarte_interfaces.hpp"

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

// Helper to register the standard suite of tests applicable to any transport
void register_standard_test_suite(TestOrchestrator& orchestrator) {
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
}

int main() {
  spdlog::set_level(spdlog::level::debug);

  // Parse configuration from toml
  toml::table config;
  try {
    config = toml::parse_file("end_to_end/config.toml");
  } catch (const toml::parse_error& err) {
    spdlog::error("Parsing config.toml failed:\n{}", err.what());
    return 1;
  }

  if (!validate_config(config)) {
    spdlog::error("Configuration validation failed. Aborting tests.");
    return 1;
  }

  auto realm = config["realm"].value<std::string>().value();
  auto device_id = config["device_id"].value<std::string>().value();
  auto astarte_base_url = config["astarte_base_url"].value<std::string>().value();
  auto appengine_token = config["appengine_token"].value<std::string>().value();

  // Create orchestrator with common HTTP config
  TestHttpConfig config_http{
      .astarte_base_url = astarte_base_url,
      .appengine_token = appengine_token,
      .realm = realm,
      .device_id = device_id,
  };

  TestOrchestrator orchestrator(config_http);

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

  // Register and run tests
  register_standard_test_suite(orchestrator);
  orchestrator.execute_all();

#else
  auto pairing_token_opt = config.at("mqtt").at_path("pairing_token").value<std::string>();
  auto credential_secret_opt = config.at("mqtt").at_path("credential_secret").value<std::string>();

  if (pairing_token_opt) {
    // Mode 1: Device Pairing (No Device Factory needed yet)
    orchestrator.execute_without_device(testcases::device_pairing(pairing_token_opt.value()));

  } else if (credential_secret_opt) {
    // Mode 2: Standard Tests using existing credentials
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

    // Register and run tests
    register_standard_test_suite(orchestrator);
    orchestrator.execute_all();

  } else {
    spdlog::error(
        "MQTT Config Error: Either 'credential_secret' or 'pairing_token' must be provided.");
    return 1;
  }
#endif

  return 0;
}
