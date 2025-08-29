// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <format>
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
  TransportConfigVariant transport_config =
      GrpcTestConfig{.server_addr = config["grpc"]["server_addr"].value<std::string>().value(),
                     .node_id = config["grpc"]["node_id"].value<std::string>().value(),
                     .interfaces = {
                         astarte_interfaces::DeviceDatastream::FILE,
                         astarte_interfaces::ServerDatastream::FILE,
                         astarte_interfaces::DeviceAggregate::FILE,
                         astarte_interfaces::ServerAggregate::FILE,
                         astarte_interfaces::DeviceProperty::FILE,
                         astarte_interfaces::ServerProperty::FILE,
                     }};

  orchestrator.with_transport_config(transport_config);

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
    // execute only pairing api tests
    orchestrator.execute_without_device(testcases::device_pairing(pairing_token_opt.value()));
  } else if (credential_secret_opt) {
    auto store_dir = config["mqtt"]["store_dir"].value<std::string>().value();

    TransportConfigVariant transport_config =
        MqttTestConfig{.cfg = MqttConfig::with_credential_secret(
                           realm, device_id, credential_secret_opt.value(),
                           astarte_fmt::format("{}/pairing", astarte_base_url), store_dir),
                       .interfaces = {
                           astarte_interfaces::DeviceDatastream::FILE,
                           astarte_interfaces::ServerDatastream::FILE,
                           astarte_interfaces::DeviceAggregate::FILE,
                           astarte_interfaces::ServerAggregate::FILE,
                           astarte_interfaces::DeviceProperty::FILE,
                           astarte_interfaces::ServerProperty::FILE,
                       }};

    orchestrator.with_transport_config(transport_config);

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
