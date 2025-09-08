// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include "case.hpp"

namespace testcases {

TestCase device_pairing() {
  return TestCase(
      "Pair a device to Astarte",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1)),

          // TODO: at the moment the connection to Atsarte is not implemented.
          // we only use this method to check whether the device has been correctly created.
          TestActionCheckDeviceStatus::Create(false, {}),
          TestActionSleep::Create(std::chrono::seconds(1)),

          TestActionDisconnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1))});
}

}  // namespace testcases
