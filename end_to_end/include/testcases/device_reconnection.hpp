// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"
#include "constants/astarte_time.hpp"

namespace testcases {

TestCase device_reconnection() {
  return TestCase(
      "Device Reconnection",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionDisconnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),

          // before reconnecting, we try to send an AstarteIndividual, which should fail
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                             AstarteDatastreamIndividual(AstarteData(12))),
              astarte_time::TIMESTAMP, true),
          TestActionSleep::Create(std::chrono::seconds(1)),

          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),

          // send an AstarteIndividual and check for success
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                             AstarteDatastreamIndividual(AstarteData(12))),
              astarte_time::TIMESTAMP),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "integer_endpoint",
                             AstarteDatastreamIndividual(AstarteData(12))),
              astarte_time::TIMESTAMP),
          TestActionSleep::Create(std::chrono::seconds(1)),

          TestActionDisconnect::Create(), TestActionSleep::Create(std::chrono::seconds(1))});
}
}  // namespace testcases
