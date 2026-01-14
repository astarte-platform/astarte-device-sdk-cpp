// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "case.hpp"
#include "constants/astarte_interfaces.hpp"
#include "constants/astarte_time.hpp"

namespace testcases {
using namespace std::chrono_literals;

TestCase device_add_remove_interface(std::string device_id) {
  return TestCase(
      "Add/remove interfaces",
      {Actions::Connect(), Actions::Sleep(1s),
       Actions::TransmitDeviceData(
           AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                          AstarteDatastreamIndividual(AstarteData(12))),
           astarte_time::TIMESTAMP),
       Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                  "/integer_endpoint",
                                                  AstartePropertyIndividual(AstarteData(12)))),

       Actions::Sleep(1s),

       Actions::RemoveInterface(std::string(astarte_interfaces::DeviceDatastream::INTERFACE)),

       Actions::Sleep(1s),

       // Should fail because interface is removed
       Actions::ExpectFailure(Actions::TransmitDeviceData(
           AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                          AstarteDatastreamIndividual(AstarteData(12))),
           astarte_time::TIMESTAMP)),

       // Property interface still there
       Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                  "/integer_endpoint",
                                                  AstartePropertyIndividual(AstarteData(12)))),

       Actions::Sleep(1s),

       Actions::AddInterfaceFile(std::string(astarte_interfaces::DeviceDatastream::FILE)),

       Actions::Sleep(1s),

       Actions::TransmitDeviceData(
           AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                          AstarteDatastreamIndividual(AstarteData(12))),
           astarte_time::TIMESTAMP),
       Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                  "/integer_endpoint",
                                                  AstartePropertyIndividual(AstarteData(12)))),

       Actions::Sleep(1s), Actions::Disconnect(), Actions::Sleep(1s)},
      device_id);
}

}  // namespace testcases
