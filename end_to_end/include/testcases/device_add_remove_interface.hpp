// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "case.hpp"
#include "constants/interfaces.hpp"
#include "constants/time.hpp"

namespace testcases {
using namespace std::chrono_literals;

TestCase device_add_remove_interface(std::string device_id) {
    return TestCase(
        "Add/remove interfaces",
        {actions::Connect(),
         actions::Sleep(1s),
         actions::TransmitDeviceData(
             Message(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 "/integer_endpoint",
                 DatastreamIndividual(Data(12))
             ),
             constants::time::TIMESTAMP
         ),
         actions::TransmitDeviceData(Message(
             constants::interfaces::DeviceProperty::INTERFACE,
             "/integer_endpoint",
             AstartePropertyIndividual(Data(12))
         )),
         actions::Sleep(1s),
         actions::RemoveInterface(std::string(constants::interfaces::DeviceDatastream::INTERFACE)),
         actions::Sleep(1s),
         actions::ExpectFailure(
             actions::TransmitDeviceData(
                 Message(
                     constants::interfaces::DeviceDatastream::INTERFACE,
                     "/integer_endpoint",
                     DatastreamIndividual(Data(12))
                 ),
                 constants::time::TIMESTAMP
             )
         ),
         actions::TransmitDeviceData(Message(
             constants::interfaces::DeviceProperty::INTERFACE,
             "/integer_endpoint",
             AstartePropertyIndividual(Data(12))
         )),
         actions::Sleep(1s),
         actions::AddInterfaceFile(std::string(constants::interfaces::DeviceDatastream::FILE)),
         actions::Sleep(1s),
         actions::TransmitDeviceData(
             Message(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 "/integer_endpoint",
                 DatastreamIndividual(Data(12))
             ),
             constants::time::TIMESTAMP
         ),
         actions::TransmitDeviceData(Message(
             constants::interfaces::DeviceProperty::INTERFACE,
             "/integer_endpoint",
             AstartePropertyIndividual(Data(12))
         )),

         actions::Sleep(1s),
         actions::Disconnect(),
         actions::Sleep(1s)},
        device_id
    );
}

}  // namespace testcases
