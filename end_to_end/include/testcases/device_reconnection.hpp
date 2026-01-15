// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "case.hpp"
#include "constants/data_sets.hpp"
#include "constants/interfaces.hpp"
#include "constants/time.hpp"

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
                     constants::interfaces::DeviceDatastream::INTERFACE,
                     constants::data_sets::Integer::ENDPOINT_FULL,
                     AstarteDatastreamIndividual(constants::data_sets::Integer::DATA)
                 ),
                 constants::time::TIMESTAMP
             )
         ),
         actions::Sleep(1s),
         actions::Connect(),
         actions::Sleep(1s),
         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::Integer::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::Integer::DATA)
             ),
             constants::time::TIMESTAMP
         ),
         actions::Sleep(1s),
         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::Integer::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::Integer::DATA)
             ),
             constants::time::TIMESTAMP
         ),
         actions::Sleep(1s),
         actions::Disconnect(),
         actions::Sleep(1s)},
        device_id
    );
}
}  // namespace testcases
