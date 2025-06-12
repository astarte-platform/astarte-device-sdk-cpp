// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

namespace testcases {
TestCase device_aggregate() {
  AstarteDatastreamObject astarte_obj = {
      {"integer_endpoint", AstarteData(12)},
      {"longinteger_endpoint", AstarteData(17179869184)},
      {"double_endpoint", AstarteData(54.4)},
      {"boolean_endpoint", AstarteData(true)},
      {"string_endpoint", AstarteData(std::string("Hello C++!"))},
      {"datetime_endpoint",
       AstarteData(std::chrono::system_clock::time_point(
           std::chrono::sys_days(std::chrono::year_month_day(
               std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
           std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))},
      {"binaryblob_endpoint", AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})},
      {"integerarray_endpoint", AstarteData(std::vector<int32_t>{13, 2})},
      {"longintegerarray_endpoint", AstarteData(std::vector<int64_t>{17179869184, 5})},
      {"doublearray_endpoint", AstarteData(std::vector<double>{0.5})},
      {"booleanarray_endpoint", AstarteData(std::vector<bool>{false, true})},
      {"stringarray_endpoint",
       AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})},
      {"binaryblobarray_endpoint",
       AstarteData(std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})},
      {"datetimearray_endpoint",
       AstarteData(std::vector<std::chrono::system_clock::time_point>{
           std::chrono::sys_days(std::chrono::year_month_day(
               std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
               std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
           std::chrono::sys_days(std::chrono::year_month_day(
               std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
               std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
       })}};

  return TestCase(
      "Send Astarte Aggregate",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),

          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_AGGREGATE, "/sensor1", astarte_obj)),

          TestActionSleep::Create(std::chrono::seconds(1)),

          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_AGGREGATE, "sensor1", astarte_obj)),

          TestActionSleep::Create(std::chrono::seconds(1)), TestActionDisconnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1))});
}
}  // namespace testcases
