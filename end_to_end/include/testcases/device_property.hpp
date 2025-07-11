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

TestCase device_property() {
  return TestCase(
      "Device property to Astarte",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),

          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/integer_endpoint",
                             AstartePropertyIndividual(AstarteData(12)))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/longinteger_endpoint",
                             AstartePropertyIndividual(AstarteData(17179869184)))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/double_endpoint",
                             AstartePropertyIndividual(AstarteData(54.4)))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/boolean_endpoint",
                             AstartePropertyIndividual(AstarteData(true)))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/string_endpoint",
                             AstartePropertyIndividual(AstarteData(std::string("Hello C++!"))))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "/datetime_endpoint",
              AstartePropertyIndividual(AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "/binaryblob_endpoint",
              AstartePropertyIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "/integerarray_endpoint",
              AstartePropertyIndividual(AstarteData(std::vector<int32_t>{13, 2})))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "/longintegerarray_endpoint",
              AstartePropertyIndividual(AstarteData(std::vector<int64_t>{17179869184, 5})))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/doublearray_endpoint",
                             AstartePropertyIndividual(AstarteData(std::vector<double>{0.5})))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "/booleanarray_endpoint",
              AstartePropertyIndividual(AstarteData(std::vector<bool>{false, true})))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "/stringarray_endpoint",
              AstartePropertyIndividual(
                  AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "/datetimearray_endpoint",
              AstartePropertyIndividual(AstarteData(std::vector<
                                                    std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              })))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "/binaryblobarray_endpoint",
              AstartePropertyIndividual(AstarteData(
                  std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),

          TestActionSleep::Create(std::chrono::seconds(1)),

          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "integer_endpoint",
                             AstartePropertyIndividual(AstarteData(12)))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "longinteger_endpoint",
                             AstartePropertyIndividual(AstarteData(17179869184)))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "double_endpoint",
                             AstartePropertyIndividual(AstarteData(54.4)))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "boolean_endpoint",
                             AstartePropertyIndividual(AstarteData(true)))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "string_endpoint",
                             AstartePropertyIndividual(AstarteData(std::string("Hello C++!"))))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "datetime_endpoint",
              AstartePropertyIndividual(AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min))))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "binaryblob_endpoint",
              AstartePropertyIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "integerarray_endpoint",
                             AstartePropertyIndividual(AstarteData(std::vector<int32_t>{13, 2})))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "longintegerarray_endpoint",
              AstartePropertyIndividual(AstarteData(std::vector<int64_t>{17179869184, 5})))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "doublearray_endpoint",
                             AstartePropertyIndividual(AstarteData(std::vector<double>{0.5})))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "booleanarray_endpoint",
              AstartePropertyIndividual(AstarteData(std::vector<bool>{false, true})))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "stringarray_endpoint",
              AstartePropertyIndividual(
                  AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "datetimearray_endpoint",
              AstartePropertyIndividual(
                  AstarteData(std::vector<std::chrono::system_clock::time_point>{
                      std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min,
                      std::chrono::sys_days{1985y / 5 / 22} + 12s,
                  })))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "binaryblobarray_endpoint",
              AstartePropertyIndividual(AstarteData(
                  std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),

          TestActionSleep::Create(std::chrono::seconds(1)),

          // unsetting properties
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/integer_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/longinteger_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/double_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/boolean_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/string_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/datetime_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/binaryblob_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                             "/integerarray_endpoint", AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DeviceProperty::INTERFACE, "/longintegerarray_endpoint",
              AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/doublearray_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                             "/booleanarray_endpoint", AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/stringarray_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                             "/datetimearray_endpoint", AstartePropertyIndividual(std::nullopt))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                             "/binaryblobarray_endpoint", AstartePropertyIndividual(std::nullopt))),

          TestActionSleep::Create(std::chrono::seconds(1)),

          // check unset device property
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "integer_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "longinteger_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "double_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "boolean_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "string_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "datetime_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "binaryblob_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "integerarray_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                             "longintegerarray_endpoint", AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "doublearray_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "booleanarray_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "stringarray_endpoint",
                             AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                             "datetimearray_endpoint", AstartePropertyIndividual(std::nullopt))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                             "binaryblobarray_endpoint", AstartePropertyIndividual(std::nullopt))),

          TestActionSleep::Create(std::chrono::seconds(1)), TestActionDisconnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1))});
}
}  // namespace testcases
