// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"
#include "constants/astarte_time.hpp"

namespace testcases {
using namespace std::chrono_literals;

TestCase device_add_remove_interface() {
  return TestCase(
      "Add/remove interfaces",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                             AstarteDatastreamIndividual(AstarteData(12))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/integer_endpoint",
                             AstartePropertyIndividual(AstarteData(12)))),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionRemoveInterface::Create(astarte_interfaces::DeviceDatastream::INTERFACE),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                             AstarteDatastreamIndividual(AstarteData(12))),
              astarte_time::TIMESTAMP, true),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/integer_endpoint",
                             AstartePropertyIndividual(AstarteData(12)))),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionAddInterfaceFile::Create(astarte_interfaces::DeviceDatastream::FILE),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                             AstarteDatastreamIndividual(AstarteData(12))),
              astarte_time::TIMESTAMP),
          TestActionTransmitMqttData::Create(
              AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/integer_endpoint",
                             AstartePropertyIndividual(AstarteData(12)))),
          TestActionSleep::Create(std::chrono::seconds(1)), TestActionDisconnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1))});
}

}  // namespace testcases
