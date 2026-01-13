// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "astarte_device_sdk/device.hpp"
#include "exceptions.hpp"  // Added include

#ifdef ASTARTE_TRANSPORT_GRPC
#include "astarte_device_sdk/grpc/device_grpc.hpp"
#else  // ASTARTE_TRANSPORT_GRPC
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#endif  // ASTARTE_TRANSPORT_GRPC

using AstarteDeviceSdk::AstarteDevice;

#ifdef ASTARTE_TRANSPORT_GRPC
using AstarteDeviceSdk::AstarteDeviceGrpc;
#else   // ASTARTE_TRANSPORT_GRPC
using AstarteDeviceSdk::AstarteDeviceMqtt;
using AstarteDeviceSdk::config::MqttConfig;
#endif  // ASTARTE_TRANSPORT_GRPC

class TestDeviceFactory {
 public:
  virtual ~TestDeviceFactory() = default;
  virtual std::shared_ptr<AstarteDevice> create_device() const = 0;
};

// config structures
#ifdef ASTARTE_TRANSPORT_GRPC
struct TestGrpcDeviceConfig {
  std::string server_addr;
  std::string node_id;
  std::vector<std::filesystem::path> interfaces;
};

class TestGrpcDeviceFactory : public TestDeviceFactory {
 public:
  explicit TestGrpcDeviceFactory(TestGrpcDeviceConfig config) : config_(std::move(config)) {}
  std::shared_ptr<AstarteDevice> create_device() const override {
    auto device = std::make_shared<AstarteDeviceGrpc>(config_.server_addr, config_.node_id);
    for (const auto& interface_path : config_.interfaces) {
      auto res = device->add_interface_from_file(interface_path);
      if (!res) {
        throw EndToEndAstarteDeviceException(astarte_fmt::format(
            "Failed to load interface {}: {}", interface_path.string(), res.error()));
      }
    }
    return device;
  }

 private:
  TestGrpcDeviceConfig config_;
};
#else  // ASTARTE_TRANSPORT_GRPC
struct TestMqttDeviceConfig {
  std::string realm;
  std::string device_id;
  std::string credential_secret;
  std::string pairing_url;
  std::string store_dir;
  std::vector<std::filesystem::path> interfaces;
};

class TestMqttDeviceFactory : public TestDeviceFactory {
 public:
  explicit TestMqttDeviceFactory(TestMqttDeviceConfig config) : config_(std::move(config)) {}

  std::shared_ptr<AstarteDevice> create_device() const override {
    auto mqtt_config = MqttConfig::with_credential_secret(config_.realm, config_.device_id,
                                                          config_.credential_secret,
                                                          config_.pairing_url, config_.store_dir);

    auto result = AstarteDeviceSdk::AstarteDeviceMqtt::create(std::move(mqtt_config));
    if (!result) {
      throw EndToEndAstarteDeviceException(
          astarte_fmt::format("Failed to create MQTT device: {}", result.error()));
    }

    auto device = std::make_shared<AstarteDeviceMqtt>(*std::move(result));
    for (const auto& interface_path : config_.interfaces) {
      auto res = device->add_interface_from_file(interface_path);
      if (!res) {
        throw EndToEndAstarteDeviceException(astarte_fmt::format(
            "Failed to load interface {}: {}", interface_path.string(), res.error()));
      }
    }

    return device;
  }

 private:
  TestMqttDeviceConfig config_;
};

#endif  // ASTARTE_TRANSPORT_GRPC
