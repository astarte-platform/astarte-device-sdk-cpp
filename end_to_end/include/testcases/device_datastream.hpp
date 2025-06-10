// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

using AstarteDeviceSdk::AstarteMessageType;

namespace testcases {
TestCase device_datastream() {
  return TestCase(
      "Device to Astarte",
      std::vector<std::shared_ptr<TestAction>>{
          std::make_shared<TestActionConnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/integer_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(12))),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/longinteger_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(17179869184))),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/double_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(54.4))),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/boolean_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(true))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/string_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(std::string("Hello C++!")))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/datetime_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0))))),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/binaryblob_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5}))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/integerarray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(std::vector<int32_t>{13, 2}))),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/longintegerarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<int64_t>{17179869184, 5}))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/doublearray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(std::vector<double>{0.5}))),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/booleanarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<bool>{false, true}))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/stringarray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"}))),
          std::make_shared<TestActionTransmitMQTTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/datetimearray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::vector<std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              }))),
          std::make_shared<TestActionTransmitMQTTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/binaryblobarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5},
                                                                           {0x43, 0xF3, 0x00}}))),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "integer_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(12))),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "longinteger_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(17179869184))),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "double_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(54.4))),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "boolean_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(true))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "string_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(std::string("Hello C++!")))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "datetime_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0))))),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "binaryblob_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5}))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "integerarray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(std::vector<int32_t>{13, 2}))),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "longintegerarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<int64_t>{17179869184, 5}))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "doublearray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(std::vector<double>{0.5}))),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "booleanarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<bool>{false, true}))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "stringarray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"}))),
          std::make_shared<TestActionFetchRESTData>(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "datetimearray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::vector<std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              }))),
          std::make_shared<TestActionFetchRESTData>(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "binaryblobarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5},
                                                                           {0x43, 0xF3, 0x00}}))),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionDisconnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1))});
}
}  // namespace testcases
