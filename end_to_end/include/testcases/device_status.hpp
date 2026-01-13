// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

namespace testcases {

using namespace std::chrono_literals;

TestCase device_status() {
  return TestCase(
      "Device status",
      {Actions::Connect(), Actions::Sleep(1s),
       Actions::CheckDeviceStatus(
           true,
           std::vector<std::string>{std::string(astarte_interfaces::DeviceDatastream::INTERFACE),
                                    std::string(astarte_interfaces::ServerDatastream::INTERFACE),
                                    std::string(astarte_interfaces::DeviceAggregate::INTERFACE),
                                    std::string(astarte_interfaces::ServerAggregate::INTERFACE),
                                    std::string(astarte_interfaces::DeviceProperty::INTERFACE),
                                    std::string(astarte_interfaces::ServerProperty::INTERFACE)}),
       Actions::Disconnect(), Actions::Sleep(1s)});
}
}  // namespace testcases
