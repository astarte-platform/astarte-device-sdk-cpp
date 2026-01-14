// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

namespace testcases {

using namespace std::chrono_literals;

TestCase server_datastream(std::string device_id) {
  return TestCase(
      "Astarte to device",
      {Actions::Connect(),
       Actions::Sleep(1s),

       Actions::TransmitRESTData(
           AstarteMessage(astarte_interfaces::ServerDatastream::INTERFACE, "/integer_endpoint",
                          AstarteDatastreamIndividual(AstarteData((int32_t)43)))),
       Actions::TransmitRESTData(
           AstarteMessage(astarte_interfaces::ServerDatastream::INTERFACE, "/longinteger_endpoint",
                          AstarteDatastreamIndividual(AstarteData((int64_t)17179869184)))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerDatastream::INTERFACE,
                                                "/double_endpoint",
                                                AstarteDatastreamIndividual(AstarteData(4245.23)))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerDatastream::INTERFACE,
                                                "/boolean_endpoint",
                                                AstarteDatastreamIndividual(AstarteData(false)))),
       Actions::TransmitRESTData(
           AstarteMessage(astarte_interfaces::ServerDatastream::INTERFACE, "/string_endpoint",
                          AstarteDatastreamIndividual(AstarteData(std::string("Hello world"))))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/datetime_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::chrono::system_clock::time_point(
               std::chrono::sys_days(std::chrono::year_month_day(
                   std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
               std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/binaryblob_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/integerarray_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::vector<int32_t>{32, 43, 0, 3332})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/longintegerarray_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::vector<int64_t>{17179869184, 0, 6486})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/doublearray_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::vector<double>{0.0, 23.2})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/booleanarray_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::vector<bool>{false, true, true})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/stringarray_endpoint",
           AstarteDatastreamIndividual(
               AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/datetimearray_endpoint",
           AstarteDatastreamIndividual(
               AstarteData(std::vector<std::chrono::system_clock::time_point>{
                   std::chrono::sys_days(std::chrono::year_month_day(
                       std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                       std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                   std::chrono::sys_days(std::chrono::year_month_day(
                       std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                       std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
               })))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/binaryblobarray_endpoint",
           AstarteDatastreamIndividual(AstarteData(
               std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),

       Actions::Sleep(1s),

       Actions::ReadReceivedDeviceData(
           AstarteMessage(astarte_interfaces::ServerDatastream::INTERFACE, "/integer_endpoint",
                          AstarteDatastreamIndividual(AstarteData((int32_t)43)))),
       Actions::ReadReceivedDeviceData(
           AstarteMessage(astarte_interfaces::ServerDatastream::INTERFACE, "/longinteger_endpoint",
                          AstarteDatastreamIndividual(AstarteData((int64_t)17179869184)))),
       Actions::ReadReceivedDeviceData(
           AstarteMessage(astarte_interfaces::ServerDatastream::INTERFACE, "/double_endpoint",
                          AstarteDatastreamIndividual(AstarteData(4245.23)))),
       Actions::ReadReceivedDeviceData(
           AstarteMessage(astarte_interfaces::ServerDatastream::INTERFACE, "/boolean_endpoint",
                          AstarteDatastreamIndividual(AstarteData(false)))),
       Actions::ReadReceivedDeviceData(
           AstarteMessage(astarte_interfaces::ServerDatastream::INTERFACE, "/string_endpoint",
                          AstarteDatastreamIndividual(AstarteData(std::string("Hello world"))))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/datetime_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::chrono::system_clock::time_point(
               std::chrono::sys_days(std::chrono::year_month_day(
                   std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
               std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/binaryblob_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/integerarray_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::vector<int32_t>{32, 43, 0, 3332})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/longintegerarray_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::vector<int64_t>{17179869184, 0, 6486})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/doublearray_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::vector<double>{0.0, 23.2})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/booleanarray_endpoint",
           AstarteDatastreamIndividual(AstarteData(std::vector<bool>{false, true, true})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/stringarray_endpoint",
           AstarteDatastreamIndividual(
               AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/datetimearray_endpoint",
           AstarteDatastreamIndividual(
               AstarteData(std::vector<std::chrono::system_clock::time_point>{
                   std::chrono::sys_days(std::chrono::year_month_day(
                       std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                       std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                   std::chrono::sys_days(std::chrono::year_month_day(
                       std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                       std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
               })))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerDatastream::INTERFACE, "/binaryblobarray_endpoint",
           AstarteDatastreamIndividual(AstarteData(
               std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),

       Actions::Sleep(1s),
       Actions::Disconnect(),
       Actions::Sleep(1s)},
      device_id);
}
}  // namespace testcases
