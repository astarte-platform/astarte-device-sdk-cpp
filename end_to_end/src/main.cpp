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
#include "constants/file_paths.hpp"
#include "constants/interfaces.hpp"

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
  } else {
    if (!config["mqtt"]["store_dir"].is_string()) {
      spdlog::error("Configuration Error: Missing 'store_dir' inside [mqtt] table.");
      is_valid = false;
    }
    if (!config["mqtt"]["pairing_token"].is_string()) {
      spdlog::error("Configuration Error: Missing 'pairing_token' inside [mqtt] table.");
      is_valid = false;
    }
    if (!config["mqtt"]["device_id_registration"].is_string()) {
      spdlog::error("Configuration Error: Missing 'device_id_registration' inside [mqtt] table.");
      is_valid = false;
    }
    if (!config["mqtt"]["credential_secret"].is_string()) {
      spdlog::error("Configuration Error: Missing 'credential_secret' inside [mqtt] table.");
      is_valid = false;
    }
  }
#endif

  return is_valid;
}

// Helper to register the standard suite of tests applicable to any transport
void register_standard_test_suite(TestOrchestrator& orchestrator, std::string device_id) {
  orchestrator.add_test_case(testcases::device_status(device_id));
  orchestrator.add_test_case(testcases::device_reconnection(device_id));
  orchestrator.add_test_case(testcases::device_add_remove_interface(device_id));
  orchestrator.add_test_case(testcases::device_datastream(device_id));
  orchestrator.add_test_case(testcases::server_datastream(device_id));
  orchestrator.add_test_case(testcases::device_aggregate(device_id));
  orchestrator.add_test_case(testcases::server_aggregate(device_id));
  orchestrator.add_test_case(testcases::device_property(device_id));
  orchestrator.add_test_case(testcases::device_property_getter(device_id));
  orchestrator.add_test_case(testcases::server_property(device_id));
  orchestrator.add_test_case(testcases::server_property_on_new_device(device_id));
}

int main() {
  spdlog::set_level(spdlog::level::debug);

  // Parse configuration from toml
  toml::table config;
  try {
    config = toml::parse_file(constants::file_paths::CONFIG);
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
  };

  std::shared_ptr<TestDeviceFactory> device_factory = nullptr;
#ifdef ASTARTE_TRANSPORT_GRPC
  TestGrpcDeviceConfig grpc_conf = {
      .server_addr = config["grpc"]["server_addr"].value<std::string>().value(),
      .node_id = config["grpc"]["node_id"].value<std::string>().value(),
      .interfaces = {
          constants::interfaces::DeviceDatastream::FILE,
          constants::interfaces::ServerDatastream::FILE,
          constants::interfaces::DeviceAggregate::FILE,
          constants::interfaces::ServerAggregate::FILE,
          constants::interfaces::DeviceProperty::FILE,
          constants::interfaces::ServerProperty::FILE,
      }};

  device_factory = std::make_shared<TestGrpcDeviceFactory>(grpc_conf);
#else
  auto store_dir = config["mqtt"]["store_dir"].value<std::string>().value();
  auto pairing_token_opt = config["mqtt"]["pairing_token"].value<std::string>();
  auto device_id_registration_opt = config["mqtt"]["device_id_registration"].value<std::string>();
  auto credential_secret_opt = config["mqtt"]["credential_secret"].value<std::string>();

  TestMqttDeviceConfig mqtt_conf = {
      .realm = realm,
      .device_id = device_id,
      .credential_secret = credential_secret_opt.value(),
      .pairing_url = astarte_fmt::format("{}/pairing", astarte_base_url),
      .store_dir = store_dir,
      .interfaces = {
          constants::interfaces::DeviceDatastream::FILE,
          constants::interfaces::ServerDatastream::FILE,
          constants::interfaces::DeviceAggregate::FILE,
          constants::interfaces::ServerAggregate::FILE,
          constants::interfaces::DeviceProperty::FILE,
          constants::interfaces::ServerProperty::FILE,
      }};
  device_factory = std::make_shared<TestMqttDeviceFactory>(std::move(mqtt_conf));
#endif

  TestOrchestrator orchestrator(config_http, device_factory);

#ifdef ASTARTE_TRANSPORT_GRPC
  register_standard_test_suite(orchestrator, device_id);
#else
  orchestrator.add_test_case(
      testcases::device_pairing(pairing_token_opt.value(), device_id_registration_opt.value()));
  // Standard tests using existing credentials
  orchestrator.add_test_case(testcases::device_status(device_id));
  // Other device functionalities have to be implemented
  // register_standard_test_suite(orchestrator);
#endif
  orchestrator.execute_all();

  return 0;
}
