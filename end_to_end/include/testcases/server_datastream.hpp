// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

using AstarteDeviceSdk::AstarteMessageType;

namespace testcases {
TestCase server_datastream() {
  return TestCase(
      "Astarte to device",
      std::vector<std::shared_ptr<TestAction>>{
          std::make_shared<TestActionConnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/integer_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData((int32_t)43))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              astarte_interfaces::SERVER_DATASTREAM, "/longinteger_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData((int64_t)17179869184))),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/double_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(4245.23))),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/boolean_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(false))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              astarte_interfaces::SERVER_DATASTREAM, "/string_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(std::string("Hello world")))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              astarte_interfaces::SERVER_DATASTREAM, "/datetime_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0))))),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/binaryblob_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5}))),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/integerarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<int32_t>{32, 43, 0, 3332}))),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/longintegerarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<int64_t>{17179869184, 0, 6486}))),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/doublearray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<double>{0.0, 23.2}))),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/booleanarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<bool>{false, true, true}))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              astarte_interfaces::SERVER_DATASTREAM, "/stringarray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"}))),
          std::make_shared<TestActionTransmitRESTData>(AstarteMessage(
              astarte_interfaces::SERVER_DATASTREAM, "/datetimearray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::vector<std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              }))),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/binaryblobarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5},
                                                                           {0x43, 0xF3, 0x00}}))),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionReadReceivedMQTTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/integer_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData((int32_t)43))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              astarte_interfaces::SERVER_DATASTREAM, "/longinteger_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData((int64_t)17179869184))),
          std::make_shared<TestActionReadReceivedMQTTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/double_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(4245.23))),
          std::make_shared<TestActionReadReceivedMQTTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/boolean_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(false))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              astarte_interfaces::SERVER_DATASTREAM, "/string_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL, AstarteData(std::string("Hello world")))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              astarte_interfaces::SERVER_DATASTREAM, "/datetime_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0))))),
          std::make_shared<TestActionReadReceivedMQTTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/binaryblob_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5}))),
          std::make_shared<TestActionReadReceivedMQTTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/integerarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<int32_t>{32, 43, 0, 3332}))),
          std::make_shared<TestActionReadReceivedMQTTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/longintegerarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<int64_t>{17179869184, 0, 6486}))),
          std::make_shared<TestActionReadReceivedMQTTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/doublearray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<double>{0.0, 23.2}))),
          std::make_shared<TestActionReadReceivedMQTTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/booleanarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<bool>{false, true, true}))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              astarte_interfaces::SERVER_DATASTREAM, "/stringarray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"}))),
          std::make_shared<TestActionReadReceivedMQTTData>(AstarteMessage(
              astarte_interfaces::SERVER_DATASTREAM, "/datetimearray_endpoint",
              AstarteMessageType::DATASTREAM_INDIVIDUAL,
              AstarteData(std::vector<std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              }))),
          std::make_shared<TestActionTransmitRESTData>(
              AstarteMessage(astarte_interfaces::SERVER_DATASTREAM, "/binaryblobarray_endpoint",
                             AstarteMessageType::DATASTREAM_INDIVIDUAL,
                             AstarteData(std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5},
                                                                           {0x43, 0xF3, 0x00}}))),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionDisconnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1))});
}
}  // namespace testcases
