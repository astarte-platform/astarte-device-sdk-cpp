// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <toml++/toml.hpp>

#include "astarte_device_sdk/data.hpp"
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

int main() {
  spdlog::set_level(spdlog::level::debug);

  // Parse configuration from toml
  toml::table config = toml::parse_file("end_to_end/config.toml");

  auto realm = config["realm"].value<std::string>().value();
  auto device_id = config["device_id"].value<std::string>().value();

  // Create configuration structs
#ifdef ASTARTE_TRANSPORT_GRPC
  TransportConfigVariant transport_config =
      GRPCConfig{.server_addr = config["grpc"]["server_addr"].value<std::string>().value(),
                 .node_id = config["grpc"]["node_id"].value<std::string>().value(),
                 .interfaces = {
                     astarte_interfaces::DeviceDatastream::FILE,
                     astarte_interfaces::ServerDatastream::FILE,
                     astarte_interfaces::DeviceAggregate::FILE,
                     astarte_interfaces::ServerAggregate::FILE,
                     astarte_interfaces::DeviceProperty::FILE,
                     astarte_interfaces::ServerProperty::FILE,
                 }};
#else   // ASTARTE_TRANSPORT_GRPC
  auto pairing_url = config["mqtt"]["pairing_url"].value<std::string>().value();
  auto store_dir = config["mqtt"]["store_dir"].value<std::string>().value();
  auto credential_secret_opt = config.at("mqtt").at_path("credential_secret").value<std::string>();
  auto pairing_token_opt = config.at("mqtt").at_path("pairing_jwt").value<std::string>();

  auto cfg = [&] {
    if (credential_secret_opt) {
      return MqttConfig::with_credential_secret(realm, device_id, credential_secret_opt.value(),
                                                pairing_url, store_dir);
    } else if (pairing_token_opt) {
      return MqttConfig::with_pairing_token(realm, device_id, pairing_token_opt.value(),
                                            pairing_url, store_dir);
    } else {
      throw EndToEndConfigException(
          "MQTT Configuration failed, credential secret or pairing token missing or incorrect.");
    }
  };

  TransportConfigVariant transport_config =
      MQTTConfig{.cfg = cfg(),
                 .interfaces = {
                     astarte_interfaces::DeviceDatastream::FILE,
                     astarte_interfaces::ServerDatastream::FILE,
                     astarte_interfaces::DeviceAggregate::FILE,
                     astarte_interfaces::ServerAggregate::FILE,
                     astarte_interfaces::DeviceProperty::FILE,
                     astarte_interfaces::ServerProperty::FILE,
                 }};
#endif  // ASTARTE_TRANSPORT_GRPC

  struct CURLConfig curl_config = {
      .appengine_url = config["appengine_url"].value<std::string>().value(),
      .appengine_token = config["appengine_token"].value<std::string>().value(),
      .realm = realm,
      .device_id = device_id};

  // Create orchestrator
  TestOrchestrator orchestrator(transport_config, curl_config);

// Add test cases
#ifdef ASTARTE_TRANSPORT_GRPC
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
#else   // ASTARTE_TRANSPORT_GRPC
  orchestrator.add_test_case(testcases::device_pairing());
#endif  // ASTARTE_TRANSPORT_GRPC

  // Execute all test cases
  orchestrator.execute_all();

  return 0;
}
