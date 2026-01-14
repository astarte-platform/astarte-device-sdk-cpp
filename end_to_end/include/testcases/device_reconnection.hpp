// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "case.hpp"
#include "constants/astarte_interfaces.hpp"
#include "constants/astarte_time.hpp"

namespace testcases {

using namespace std::chrono_literals;

TestCase device_reconnection(std::string device_id) {
  return TestCase(
      "Device Reconnection",
      {Actions::Connect(), Actions::Sleep(1s), Actions::Disconnect(), Actions::Sleep(1s),
       Actions::ExpectFailure(Actions::TransmitDeviceData(
           AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                          AstarteDatastreamIndividual(AstarteData(12))),
           astarte_time::TIMESTAMP)),
       Actions::Sleep(1s), Actions::Connect(), Actions::Sleep(1s),
       Actions::TransmitDeviceData(
           AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "/integer_endpoint",
                          AstarteDatastreamIndividual(AstarteData(12))),
           astarte_time::TIMESTAMP),
       Actions::Sleep(1s),
       Actions::FetchRESTData(
           AstarteMessage(astarte_interfaces::DeviceDatastream::INTERFACE, "integer_endpoint",
                          AstarteDatastreamIndividual(AstarteData(12))),
           astarte_time::TIMESTAMP),
       Actions::Sleep(1s), Actions::Disconnect(), Actions::Sleep(1s)},
      device_id);
}
}  // namespace testcases
