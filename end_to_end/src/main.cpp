// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <toml++/toml.hpp>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "orchestrator.hpp"
#include "testcases/device_datastream.hpp"
#include "testcases/device_status.hpp"
#include "testcases/server_datastream.hpp"

using AstarteDeviceSdk::AstarteData;
using AstarteDeviceSdk::AstarteMessage;
using AstarteDeviceSdk::AstarteMessageType;

int main() {
  toml::table config = toml::parse_file("end_to_end/config.toml");

  // Create orchestrator and add test cases
  TestOrchestrator orchestrator(
      {.server_addr = config["server_addr"].value<std::string>().value(),
       .node_id = config["node_id"].value<std::string>().value(),
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

  TestCase device_status(testcases::device_status());
  orchestrator.add_test_case(device_status);
  TestCase device_datastream(testcases::device_datastream());
  orchestrator.add_test_case(device_datastream);
  TestCase server_datastream(testcases::server_datastream());
  orchestrator.add_test_case(server_datastream);

  // Execute all test cases
  orchestrator.execute_all();

  std::exit(0);
}
