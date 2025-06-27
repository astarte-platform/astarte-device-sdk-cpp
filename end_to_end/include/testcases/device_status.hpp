// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

namespace testcases {
TestCase device_status() {
  return TestCase(
      "Device status",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionCheckDeviceStatus::Create(
              true,
              std::vector<std::string>{std::string(astarte_interfaces::DeviceDatastream::INTERFACE),
                                       std::string(astarte_interfaces::ServerDatastream::INTERFACE),
                                       std::string(astarte_interfaces::DeviceAggregate::INTERFACE),
                                       std::string(astarte_interfaces::ServerAggregate::INTERFACE),
                                       std::string(astarte_interfaces::DeviceProperty::INTERFACE),
                                       std::string(astarte_interfaces::ServerProperty::INTERFACE)}),
          TestActionDisconnect::Create(), TestActionSleep::Create(std::chrono::seconds(1))});
}
}  // namespace testcases
