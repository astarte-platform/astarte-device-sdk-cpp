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
        {actions::Connect(),
         actions::Sleep(1s),
         actions::TransmitDeviceData(
             AstarteMessage(
                 astarte_interfaces::DeviceDatastream::INTERFACE,
                 "/integer_endpoint",
                 AstarteDatastreamIndividual(AstarteData(12))
             ),
             astarte_time::TIMESTAMP
         ),
         actions::TransmitDeviceData(AstarteMessage(
             astarte_interfaces::DeviceProperty::INTERFACE,
             "/integer_endpoint",
             AstartePropertyIndividual(AstarteData(12))
         )),
         actions::Sleep(1s),
         actions::RemoveInterface(std::string(astarte_interfaces::DeviceDatastream::INTERFACE)),
         actions::Sleep(1s),
         actions::ExpectFailure(
             actions::TransmitDeviceData(
                 AstarteMessage(
                     astarte_interfaces::DeviceDatastream::INTERFACE,
                     "/integer_endpoint",
                     AstarteDatastreamIndividual(AstarteData(12))
                 ),
                 astarte_time::TIMESTAMP
             )
         ),
         actions::TransmitDeviceData(AstarteMessage(
             astarte_interfaces::DeviceProperty::INTERFACE,
             "/integer_endpoint",
             AstartePropertyIndividual(AstarteData(12))
         )),
         actions::Sleep(1s),
         actions::AddInterfaceFile(std::string(astarte_interfaces::DeviceDatastream::FILE)),
         actions::Sleep(1s),
         actions::TransmitDeviceData(
             AstarteMessage(
                 astarte_interfaces::DeviceDatastream::INTERFACE,
                 "/integer_endpoint",
                 AstarteDatastreamIndividual(AstarteData(12))
             ),
             astarte_time::TIMESTAMP
         ),
         actions::TransmitDeviceData(AstarteMessage(
             astarte_interfaces::DeviceProperty::INTERFACE,
             "/integer_endpoint",
             AstartePropertyIndividual(AstarteData(12))
         )),

         actions::Sleep(1s),
         actions::Disconnect(),
         actions::Sleep(1s)},
        device_id
    );
}

}  // namespace testcases
