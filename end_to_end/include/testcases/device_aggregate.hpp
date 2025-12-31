// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

namespace testcases {
using namespace std::chrono_literals;

TestCase device_aggregate() {
  // TODO: at the moment, the object we sent doesn't contain neither the longinteger_endpoint nor
  // the longintegerarray_endpoint due to an error in the Appengine API, depicted in details in
  // the issue [#1355](https://github.com/astarte-platform/astarte/issues/1355).
  // Once solved the issue it is possible to introduce the missing endpoints.
  AstarteDatastreamObject astarte_obj = {
      {"integer_endpoint", AstarteData(12)},
      {"double_endpoint", AstarteData(54.4)},
      {"boolean_endpoint", AstarteData(true)},
      {"string_endpoint", AstarteData(std::string("Hello C++!"))},
      {"datetime_endpoint", AstarteData(std::chrono::system_clock::time_point(
                                std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min))},
      {"binaryblob_endpoint", AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})},
      {"integerarray_endpoint", AstarteData(std::vector<int32_t>{13, 2})},
      {"doublearray_endpoint", AstarteData(std::vector<double>{0.5})},
      {"booleanarray_endpoint", AstarteData(std::vector<bool>{false, true})},
      {"stringarray_endpoint",
       AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})},
      {"binaryblobarray_endpoint",
       AstarteData(std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})},
      {"datetimearray_endpoint", AstarteData(std::vector<std::chrono::system_clock::time_point>{
                                     std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min,
                                     std::chrono::sys_days{1985y / 5 / 22} + 12s,
                                 })}};

  return TestCase(
      "Send Astarte Aggregate",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),

          TestActionTransmitMqttData::Create(AstarteMessage(
              astarte_interfaces::DeviceAggregate::INTERFACE, "/sensor1", astarte_obj)),

          TestActionSleep::Create(std::chrono::seconds(1)),

          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DeviceAggregate::INTERFACE, "sensor1", astarte_obj)),

          TestActionSleep::Create(std::chrono::seconds(1)), TestActionDisconnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1))});
}
}  // namespace testcases
