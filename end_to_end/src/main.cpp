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
#include "testcases/server_aggregate.hpp"
#include "testcases/server_datastream.hpp"
#include "testcases/server_property.hpp"

using AstarteDeviceSdk::AstarteData;
using AstarteDeviceSdk::AstarteMessage;

int main() {
  spdlog::set_level(spdlog::level::debug);

  toml::table config = toml::parse_file("end_to_end/config.toml");

  // Create orchestrator and add test cases
  // TODO: change orchestrator so that it can accept eithe gRPC or MQTT configuration
  TestOrchestrator orchestrator(
      {.server_addr = config["server_addr"].value<std::string>().value(),
       .node_id = config["node_id"].value<std::string>().value(),
       .interfaces =
           {
               astarte_interfaces::DeviceDatastream::FILE,
               astarte_interfaces::ServerDatastream::FILE,
               astarte_interfaces::DeviceAggregate::FILE,
               astarte_interfaces::ServerAggregate::FILE,
               astarte_interfaces::DeviceProperty::FILE,
               astarte_interfaces::ServerProperty::FILE,
           }},
      {.appengine_url = config["appengine_url"].value<std::string>().value(),
       .appengine_token = config["appengine_token"].value<std::string>().value(),
       .realm = config["realm"].value<std::string>().value(),
       .device_id = config["device_id"].value<std::string>().value()});

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

  return 0;
}
