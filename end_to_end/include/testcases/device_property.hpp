// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"
#include "constants/astarte_time.hpp"

namespace testcases {
using namespace std::chrono_literals;

TestCase device_property() {
  return TestCase(
      "Device property to Astarte",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionTransmitMQTTData::Create(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.DevicePropertyNoSensor",
                             "/integer_endpoint", AstartePropertyIndividual(AstarteData(12)))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.DevicePropertyNoSensor",
                             "/boolean_endpoint", AstartePropertyIndividual(AstarteData(true)))),

          TestActionSleep::Create(std::chrono::seconds(1)),

          TestActionFetchRESTData::Create(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.DevicePropertyNoSensor",
                             "integer_endpoint", AstartePropertyIndividual(AstarteData(12)))),

          TestActionSleep::Create(std::chrono::seconds(1)),

          // TODO: fetch property using Appengine API

          TestActionTransmitMQTTData::Create(
              AstarteMessage("org.astarte-platform.cpp.end-to-end.DevicePropertyNoSensor",
                             "/integer_endpoint", AstartePropertyIndividual(std::nullopt))),

          // TODO: fetch unet property

          TestActionSleep::Create(std::chrono::seconds(1)), TestActionDisconnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1))});
}
}  // namespace testcases
