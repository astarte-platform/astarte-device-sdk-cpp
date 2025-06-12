// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <toml++/toml.hpp>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "orchestrator.hpp"
#include "testcases/device_aggregate.hpp"
#include "testcases/device_datastream.hpp"
#include "testcases/device_reconnection.hpp"
#include "testcases/device_status.hpp"
#include "testcases/server_datastream.hpp"

using AstarteDeviceSdk::AstarteData;
using AstarteDeviceSdk::AstarteMessage;

int main() {
  spdlog::set_level(spdlog::level::trace);

  toml::table config = toml::parse_file("end_to_end/config.toml");

  // Create orchestrator and add test cases
  TestOrchestrator orchestrator(
      {.server_addr = config["server_addr"].value<std::string>().value(),
       .node_id = config["node_id"].value<std::string>().value(),
       // TODO: modify constants file to add constants also for the file paths
       .interfaces =
           {"end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.DeviceDatastream.json",
            "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.ServerDatastream.json",
            "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.DeviceProperty.json",
            "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.ServerProperty.json",
            "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.DeviceAggregate.json",
            "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.ServerAggregate.json"}},
      {.appengine_url = config["appengine_url"].value<std::string>().value(),
       .appengine_token = config["appengine_token"].value<std::string>().value(),
       .realm = config["realm"].value<std::string>().value(),
       .device_id = config["device_id"].value<std::string>().value()});

  // orchestrator.add_test_case(testcases::device_status());

  // FIXME: decomment after having resolved astarte-device-sdk-cpp issue #51
  // orchestrator.add_test_case(testcases::device_reconnection());

  // orchestrator.add_test_case(testcases::device_datastream());
  // orchestrator.add_test_case(testcases::server_datastream());

  // FIXME: fix error "unable to publish astarte message, err: Astarte(MappingNotFound { interface:
  // "org.astarte-platform.cpp.end-to-end.DeviceAggregate:0:1", mapping: "/sensor1" }) terminate
  // called after throwing an instance of 'AstarteDeviceSdk::AstarteInvalidInputException'"
  orchestrator.add_test_case(testcases::device_aggregate());

  // Execute all test cases
  orchestrator.execute_all();

  std::exit(0);
}
