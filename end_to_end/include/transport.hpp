// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// include headers
#ifdef ASTARTE_TRANSPORT_GRPC
#include "astarte_device_sdk/device_grpc.hpp"

using AstarteDeviceSdk::AstarteDeviceGRPC;
#else  // ASTARTE_TRANSPORT_GRPC
#include "astarte_device_sdk/device_mqtt.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"

using AstarteDeviceSdk::AstarteDeviceMQTT;
using AstarteDeviceSdk::MqttConfig;
#endif  // ASTARTE_TRANSPORT_GRPC

// config structures
#ifdef ASTARTE_TRANSPORT_GRPC
struct GrpcTestConfig {
  std::string server_addr;
  std::string node_id;
  std::vector<std::filesystem::path> interfaces;
};

using TransportConfigVariant = std::variant<struct GrpcTestConfig>;
#else   // ASTARTE_TRANSPORT_GRPC
struct MqttTestConfig {
  MqttConfig cfg;
  std::vector<std::filesystem::path> interfaces;
};

using TransportConfigVariant = std::variant<struct MqttTestConfig>;
#endif  // ASTARTE_TRANSPORT_GRPC
