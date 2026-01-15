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

TestCase device_reconnection(std::string device_id) {
    return TestCase(
        "Device Reconnection",
        {actions::Connect(),
         actions::Sleep(1s),
         actions::Disconnect(),
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
         actions::Sleep(1s),
         actions::Connect(),
         actions::Sleep(1s),
         actions::TransmitDeviceData(
             AstarteMessage(
                 astarte_interfaces::DeviceDatastream::INTERFACE,
                 "/integer_endpoint",
                 AstarteDatastreamIndividual(AstarteData(12))
             ),
             astarte_time::TIMESTAMP
         ),
         actions::Sleep(1s),
         actions::FetchRESTData(
             AstarteMessage(
                 astarte_interfaces::DeviceDatastream::INTERFACE,
                 "integer_endpoint",
                 AstarteDatastreamIndividual(AstarteData(12))
             ),
             astarte_time::TIMESTAMP
         ),
         actions::Sleep(1s),
         actions::Disconnect(),
         actions::Sleep(1s)},
        device_id
    );
}
}  // namespace testcases
