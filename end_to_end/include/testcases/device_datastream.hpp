// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"
#include "constants/astarte_time.hpp"

namespace testcases {
using namespace std::chrono_literals;

TestCase device_datastream() {
  return TestCase(
      "Device to Astarte",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                             AstarteDatastreamIndividual(AstarteData(12))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE,
                             "/longinteger_endpoint",
                             AstarteDatastreamIndividual(AstarteData(17179869184))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/double_endpoint",
                             AstarteDatastreamIndividual(AstarteData(54.4))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/boolean_endpoint",
                             AstarteDatastreamIndividual(AstarteData(true))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/string_endpoint",
                             AstarteDatastreamIndividual(AstarteData(std::string("Hello C++!")))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "/datetime_endpoint",
                  AstarteDatastreamIndividual(AstarteData(std::chrono::system_clock::time_point(
                      std::chrono::sys_days(std::chrono::year_month_day(
                          std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) +
                      std::chrono::seconds(0))))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "/binaryblob_endpoint",

                  AstarteDatastreamIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5}))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE,
                             "/integerarray_endpoint",
                             AstarteDatastreamIndividual(AstarteData(std::vector<int32_t>{13, 2}))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "/longintegerarray_endpoint",

                  AstarteDatastreamIndividual(AstarteData(std::vector<int64_t>{17179869184, 5}))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE,
                             "/doublearray_endpoint",
                             AstarteDatastreamIndividual(AstarteData(std::vector<double>{0.5}))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "/booleanarray_endpoint",

                  AstarteDatastreamIndividual(AstarteData(std::vector<bool>{false, true}))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "/stringarray_endpoint",

                  AstarteDatastreamIndividual(
                      AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"}))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "/datetimearray_endpoint",
                  AstarteDatastreamIndividual(AstarteData(std::vector<
                                                          std::chrono::system_clock::time_point>{
                      std::chrono::sys_days(std::chrono::year_month_day(
                          std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                          std::chrono::hours(10) + std::chrono::minutes(15) +
                          std::chrono::seconds(0),
                      std::chrono::sys_days(std::chrono::year_month_day(
                          std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                          std::chrono::hours(0) + std::chrono::minutes(0) +
                          std::chrono::seconds(12),
                  }))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "/binaryblobarray_endpoint",

                  AstarteDatastreamIndividual(AstarteData(
                      std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}}))),
              astarte_time::TIMESTAMP),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "integer_endpoint",
                             AstarteDatastreamIndividual(AstarteData(12))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE,
                             "longinteger_endpoint",
                             AstarteDatastreamIndividual(AstarteData(17179869184))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "double_endpoint",
                             AstarteDatastreamIndividual(AstarteData(54.4))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "boolean_endpoint",
                             AstarteDatastreamIndividual(AstarteData(true))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "string_endpoint",
                             AstarteDatastreamIndividual(AstarteData(std::string("Hello C++!")))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "datetime_endpoint",
                  AstarteDatastreamIndividual(AstarteData(std::chrono::system_clock::time_point(
                      std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min)))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "binaryblob_endpoint",
                  AstarteDatastreamIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5}))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE,
                             "integerarray_endpoint",
                             AstarteDatastreamIndividual(AstarteData(std::vector<int32_t>{13, 2}))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "longintegerarray_endpoint",

                  AstarteDatastreamIndividual(AstarteData(std::vector<int64_t>{17179869184, 5}))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE,
                             "doublearray_endpoint",
                             AstarteDatastreamIndividual(AstarteData(std::vector<double>{0.5}))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "booleanarray_endpoint",

                  AstarteDatastreamIndividual(AstarteData(std::vector<bool>{false, true}))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "stringarray_endpoint",
                  AstarteDatastreamIndividual(
                      AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"}))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE,
                             "datetimearray_endpoint",
                             AstarteDatastreamIndividual(
                                 AstarteData(std::vector<std::chrono::system_clock::time_point>{
                                     std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min,
                                     std::chrono::sys_days{1985y / 5 / 22} + 12s,
                                 }))),
              astarte_time::TIMESTAMP),
          TestActionFetchRESTData::Create(
              AstarteMessage(
                  astarte_interfaces::DeviceDatastream::INTERFACE, "binaryblobarray_endpoint",

                  AstarteDatastreamIndividual(AstarteData(
                      std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}}))),
              astarte_time::TIMESTAMP),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionDisconnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1))});
}
}  // namespace testcases
