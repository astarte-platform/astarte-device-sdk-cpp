// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>
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
struct GRPCConfig {
  std::string server_addr;
  std::string node_id;
  std::vector<std::filesystem::path> interfaces;
};

using TransportConfigVariant = std::variant<struct GRPCConfig>;
#else   // ASTARTE_TRANSPORT_GRPC
struct MQTTConfig {
  MqttConfig cfg;
  std::vector<std::filesystem::path> interfaces;
};

using TransportConfigVariant = std::variant<struct MQTTConfig>;
#endif  // ASTARTE_TRANSPORT_GRPC
