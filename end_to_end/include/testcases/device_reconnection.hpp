// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

namespace testcases {

TestCase device_reconnection() {
  return TestCase(
      "Device Reconnection",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionDisconnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),
          // FIXME: at the moment the test case fails since internally grpc uses the same
          // context for the connection, which should not be done as explained in
          // the documentation:
          // https://grpc.github.io/grpc/cpp/classgrpc_1_1_client_context.html#details

          // TODO: before reconnecting, try to send an AstarteIndividual and check for errors,
          // eventually modify the code to log them without failing.

          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),

          // TODO: send an AstarteIndividual and check for success

          TestActionDisconnect::Create(), TestActionSleep::Create(std::chrono::seconds(1))});
}
}  // namespace testcases
