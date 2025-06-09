// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace astarte_interfaces {
// Datastream
const std::string DEVICE_DATASTREAM =
    "org.astarte-platform.cpp.end-to-end.DeviceDatastream";
const std::string SERVER_DATASTREAM =
    "org.astarte-platform.cpp.end-to-end.ServerDatastream";

// Property
const std::string DEVICE_PROPERTY = "org.astarte-platform.cpp.end-to-end.DeviceProperty";
const std::string SERVER_PROPERTY = "org.astarte-platform.cpp.end-to-end.ServerProperty";

// Aggregate
const std::string DEVICE_AGGREGATE = "org.astarte-platform.cpp.end-to-end.DeviceAggregate";
const std::string SERVER_AGGREGATE = "org.astarte-platform.cpp.end-to-end.ServerAggregate";
}  // namespace astarte_interfaces
