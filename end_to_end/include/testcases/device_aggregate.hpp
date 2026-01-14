// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

namespace testcases {
using namespace std::chrono_literals;

TestCase device_aggregate(std::string device_id) {
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

  return TestCase("Send Astarte Aggregate",
                  {Actions::Connect(), Actions::Sleep(1s),

                   Actions::TransmitDeviceData(AstarteMessage(
                       astarte_interfaces::DeviceAggregate::INTERFACE, "/sensor1", astarte_obj)),

                   Actions::Sleep(1s),

                   Actions::FetchRESTData(AstarteMessage(
                       astarte_interfaces::DeviceAggregate::INTERFACE, "sensor1", astarte_obj)),

                   Actions::Sleep(1s), Actions::Disconnect(), Actions::Sleep(1s)},
                  device_id);
}
}  // namespace testcases
