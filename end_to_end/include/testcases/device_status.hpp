// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "case.hpp"
#include "constants/interfaces.hpp"

namespace testcases {

using namespace std::chrono_literals;

TestCase device_status(std::string device_id) {
    return TestCase(
        "Device status",
        {actions::Connect(),
         actions::Sleep(1s),
         actions::CheckDeviceStatus(
             true,
             std::vector<std::string>{
                 std::string(constants::interfaces::DeviceDatastream::INTERFACE),
                 std::string(constants::interfaces::ServerDatastream::INTERFACE),
                 std::string(constants::interfaces::DeviceAggregate::INTERFACE),
                 std::string(constants::interfaces::ServerAggregate::INTERFACE),
                 std::string(constants::interfaces::DeviceProperty::INTERFACE),
                 std::string(constants::interfaces::ServerProperty::INTERFACE)
             }
         ),
         actions::Disconnect(),
         actions::Sleep(1s)},
        device_id
    );
}
}  // namespace testcases
