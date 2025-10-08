// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifndef ASTARTE_TRANSPORT_GRPC

#include <vector>

#include "case.hpp"

namespace testcases {

TestCase device_pairing(std::string pairing_token) {
  return TestCase("Pairing", std::vector<std::shared_ptr<TestAction>>{
                                 TestActionPairingApiRegistration::Create(pairing_token),
                                 TestActionSleep::Create(std::chrono::seconds(1)),
                                 TestActionCheckDeviceStatus::Create(false),
                                 TestActionSleep::Create(std::chrono::seconds(1)),
                             });
}

}  // namespace testcases

#endif  // ASTARTE_TRANSPORT_GRPC
