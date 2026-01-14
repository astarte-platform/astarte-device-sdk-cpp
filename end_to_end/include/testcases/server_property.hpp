// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

#include "action.hpp"
#include "case.hpp"
#include "constants/astarte_interfaces.hpp"
#include "constants/astarte_time.hpp"

namespace testcases {
using namespace std::chrono_literals;

TestCase server_property(std::string device_id) {
  return TestCase(
      "Server property to Device",
      {Actions::Connect(), Actions::Sleep(1s),

       // set server properties
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/integer_endpoint",
                                                AstartePropertyIndividual(AstarteData(43)))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/longinteger_endpoint",
           AstartePropertyIndividual(AstarteData(17179869184)))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/double_endpoint",
                                                AstartePropertyIndividual(AstarteData(54.4)))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/boolean_endpoint",
                                                AstartePropertyIndividual(AstarteData(true)))),
       Actions::TransmitRESTData(
           AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/string_endpoint",
                          AstartePropertyIndividual(AstarteData(std::string("Hello C++!"))))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/datetime_endpoint",
           AstartePropertyIndividual(AstarteData(std::chrono::system_clock::time_point(
               std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min))))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/binaryblob_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/integerarray_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<int32_t>{13, 2})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/longintegerarray_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<int64_t>{17179869184, 5})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/doublearray_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<double>{0.5})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/booleanarray_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<bool>{false, true})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/stringarray_endpoint",
           AstartePropertyIndividual(
               AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/datetimearray_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<std::chrono::system_clock::time_point>{
               std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min,
               std::chrono::sys_days{1985y / 5 / 22} + 12s,
           })))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/binaryblobarray_endpoint",
           AstartePropertyIndividual(AstarteData(
               std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),

       Actions::Sleep(1s),

       // check if server property have been set
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/integer_endpoint",
                                                      AstartePropertyIndividual(AstarteData(43)))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/longinteger_endpoint",
           AstartePropertyIndividual(AstarteData(17179869184)))),
       Actions::ReadReceivedDeviceData(
           AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/double_endpoint",
                          AstartePropertyIndividual(AstarteData(54.4)))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/boolean_endpoint",
           AstartePropertyIndividual(AstarteData(true)))),
       Actions::ReadReceivedDeviceData(
           AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/string_endpoint",
                          AstartePropertyIndividual(AstarteData(std::string("Hello C++!"))))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/datetime_endpoint",
           AstartePropertyIndividual(AstarteData(std::chrono::system_clock::time_point(
               std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min))))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/binaryblob_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/integerarray_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<int32_t>{13, 2})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/longintegerarray_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<int64_t>{17179869184, 5})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/doublearray_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<double>{0.5})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/booleanarray_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<bool>{false, true})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/stringarray_endpoint",
           AstartePropertyIndividual(
               AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/datetimearray_endpoint",
           AstartePropertyIndividual(AstarteData(std::vector<std::chrono::system_clock::time_point>{
               std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min,
               std::chrono::sys_days{1985y / 5 / 22} + 12s,
           })))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/binaryblobarray_endpoint",
           AstartePropertyIndividual(AstarteData(
               std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),

       Actions::Sleep(1s),

       // unset server properties
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/integer_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/longinteger_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/double_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/boolean_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/string_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/datetime_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/binaryblob_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/integerarray_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/longintegerarray_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/doublearray_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/booleanarray_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/stringarray_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/datetimearray_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/binaryblobarray_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),

       Actions::Sleep(1s),

       // check unset server property
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/integer_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/longinteger_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/double_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/boolean_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/string_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/datetime_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/binaryblob_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/integerarray_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/longintegerarray_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/doublearray_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/booleanarray_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/stringarray_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/datetimearray_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/binaryblobarray_endpoint",
                                                      AstartePropertyIndividual(std::nullopt))),

       Actions::Sleep(1s), Actions::Disconnect(), Actions::Sleep(1s)},
      device_id);
}

TestCase server_property_on_new_device(std::string device_id) {
  return TestCase(
      "Server property to a new Device",
      {Actions::Sleep(1s), Actions::Connect(), Actions::Sleep(1s),

       // set server properties
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/integer_endpoint",
                                                AstartePropertyIndividual(AstarteData(43)))),
       Actions::TransmitRESTData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/longinteger_endpoint",
           AstartePropertyIndividual(AstarteData(17179869184)))),

       // disconnect and reconnect
       Actions::Sleep(1s), Actions::Disconnect(), Actions::Sleep(1s), Actions::Connect(),
       Actions::Sleep(1s),

       // check if server property have been received
       Actions::ReadReceivedDeviceData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                      "/sensor1/integer_endpoint",
                                                      AstartePropertyIndividual(AstarteData(43)))),
       Actions::ReadReceivedDeviceData(AstarteMessage(
           astarte_interfaces::ServerProperty::INTERFACE, "/sensor1/longinteger_endpoint",
           AstartePropertyIndividual(AstarteData(17179869184)))),

       Actions::Sleep(1s),

       // unset the properties
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/integer_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),
       Actions::TransmitRESTData(AstarteMessage(astarte_interfaces::ServerProperty::INTERFACE,
                                                "/sensor1/longinteger_endpoint",
                                                AstartePropertyIndividual(std::nullopt))),

       Actions::Sleep(1s), Actions::Disconnect(), Actions::Sleep(1s)},
      device_id);
}
}  // namespace testcases
