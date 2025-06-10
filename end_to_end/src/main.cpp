// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <toml++/toml.hpp>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "orchestrator.hpp"

using AstarteDeviceSdk::AstarteData;
using AstarteDeviceSdk::AstarteMessage;

int main() {
  // Create test cases
  auto device_status = std::make_unique<TestCase>(
      "Device status",
      std::vector<std::shared_ptr<TestAction>>{
          std::make_shared<TestActionConnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionCheckDeviceStatus>(
              true, std::vector<std::string>{"org.astarte-platform.cpp.end-to-end.DeviceDatastream",
                                             "org.astarte-platform.cpp.end-to-end.ServerDatastream",
                                             "org.astarte-platform.cpp.end-to-end.DeviceAggregate",
                                             "org.astarte-platform.cpp.end-to-end.ServerAggregate",
                                             "org.astarte-platform.cpp.end-to-end.DeviceProperty",
                                             "org.astarte-platform.cpp.end-to-end.ServerProperty"}),
          std::make_shared<TestActionDisconnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1))});

  auto tx_device_to_astarte = std::make_unique<TestCase>(
      "Device to Astarte",
      std::vector<std::shared_ptr<TestAction>>{
          std::make_shared<TestActionConnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.DeviceDatastream",
                             "/integer_endpoint", AstarteDatastreamIndividual(AstarteData(12)))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/longinteger_endpoint",
              AstarteDatastreamIndividual(AstarteData(17179869184)))),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.DeviceDatastream",
                             "/double_endpoint", AstarteDatastreamIndividual(AstarteData(54.4)))),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.DeviceDatastream",
                             "/boolean_endpoint", AstarteDatastreamIndividual(AstarteData(true)))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/string_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::string("Hello C++!"))))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/datetime_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/binaryblob_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/integerarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<int32_t>{13, 2})))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/longintegerarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<int64_t>{17179869184, 5})))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/doublearray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<double>{0.5})))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/booleanarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<bool>{false, true})))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/stringarray_endpoint",
              AstarteDatastreamIndividual(
                  AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/datetimearray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<
                                                      std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              })))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "/binaryblobarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(
                  std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.DeviceDatastream",
                             "integer_endpoint", AstarteDatastreamIndividual(AstarteData(12)))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "longinteger_endpoint",
              AstarteDatastreamIndividual(AstarteData(17179869184)))),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.DeviceDatastream",
                             "double_endpoint", AstarteDatastreamIndividual(AstarteData(54.4)))),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.DeviceDatastream",
                             "boolean_endpoint", AstarteDatastreamIndividual(AstarteData(true)))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "string_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::string("Hello C++!"))))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "datetime_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "binaryblob_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "integerarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<int32_t>{13, 2})))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "longintegerarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<int64_t>{17179869184, 5})))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "doublearray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<double>{0.5})))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "booleanarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<bool>{false, true})))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "stringarray_endpoint",

              AstarteDatastreamIndividual(
                  AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "datetimearray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<
                                                      std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              })))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.DeviceDatastream", "binaryblobarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(
                  std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionDisconnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1))});

  auto tx_astarte_to_device = std::make_unique<TestCase>(
      "Astarte to device",
      std::vector<std::shared_ptr<TestAction>>{
          std::make_shared<TestActionConnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/integer_endpoint",
              AstarteDatastreamIndividual(AstarteData((int32_t)43)))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/longinteger_endpoint",
              AstarteDatastreamIndividual(AstarteData((int64_t)17179869184)))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/double_endpoint",
              AstarteDatastreamIndividual(AstarteData(4245.23)))),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.ServerDatastream",
                             "/boolean_endpoint", AstarteDatastreamIndividual(AstarteData(false)))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/string_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::string("Hello world"))))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/datetime_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/binaryblob_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/integerarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<int32_t>{32, 43, 0, 3332})))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/longintegerarray_endpoint",
              AstarteDatastreamIndividual(
                  AstarteData(std::vector<int64_t>{17179869184, 0, 6486})))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/doublearray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<double>{0.0, 23.2})))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/booleanarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<bool>{false, true, true})))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/stringarray_endpoint",
              AstarteDatastreamIndividual(
                  AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/datetimearray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<
                                                      std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              })))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/binaryblobarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(
                  std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/integer_endpoint",
              AstarteDatastreamIndividual(AstarteData((int32_t)43)))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/longinteger_endpoint",
              AstarteDatastreamIndividual(AstarteData((int64_t)17179869184)))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/double_endpoint",
              AstarteDatastreamIndividual(AstarteData(4245.23)))),
          std::make_shared<TestActionReadReceivedMQTTData>(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.ServerDatastream",
                             "/boolean_endpoint", AstarteDatastreamIndividual(AstarteData(false)))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/string_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::string("Hello world"))))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/datetime_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/binaryblob_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/integerarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<int32_t>{32, 43, 0, 3332})))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/longintegerarray_endpoint",
              AstarteDatastreamIndividual(
                  AstarteData(std::vector<int64_t>{17179869184, 0, 6486})))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/doublearray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<double>{0.0, 23.2})))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/booleanarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<bool>{false, true, true})))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/stringarray_endpoint",
              AstarteDatastreamIndividual(
                  AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/datetimearray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<
                                                      std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              })))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              "org.astarte-platform.cpp.end-to-end.ServerDatastream", "/binaryblobarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(
                  std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionDisconnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1))});

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
  orchestrator.add_test_case(device_status);
  orchestrator.add_test_case(tx_device_to_astarte);
  orchestrator.add_test_case(tx_astarte_to_device);

  // Execute all test cases
  orchestrator.execute_all();

  std::exit(0);
}
