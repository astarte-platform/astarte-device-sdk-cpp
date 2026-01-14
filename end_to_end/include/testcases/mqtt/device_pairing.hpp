// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifndef ASTARTE_TRANSPORT_GRPC

#include <vector>

#include "action_mqtt.hpp"
#include "case.hpp"

namespace testcases {

using namespace std::chrono_literals;

TestCase device_pairing(std::string pairing_token, std::string device_id) {
  return TestCase("Pairing",
                  {
                      Actions::RegisterDevice(pairing_token),
                      Actions::Sleep(1s),
                      Actions::CheckDeviceStatus(false),
                      Actions::Sleep(1s),
                  },
                  device_id, false);
}

}  // namespace testcases

#endif  // ASTARTE_TRANSPORT_GRPC
