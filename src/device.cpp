// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/device.hpp"

#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "device_impl.hpp"

namespace AstarteDeviceSdk {

AstarteDevice::AstarteDevice(const std::string &server_addr, const std::string &node_uuid)
    : astarte_device_impl_{std::make_shared<AstarteDeviceImpl>(server_addr, node_uuid)} {}

AstarteDevice::~AstarteDevice() = default;

void AstarteDevice::add_interface_from_json(const std::filesystem::path &json_file) {
  astarte_device_impl_->add_interface_from_json(json_file);
}

void AstarteDevice::connect() { astarte_device_impl_->connect(); }

void AstarteDevice::disconnect() { astarte_device_impl_->disconnect(); }

void AstarteDevice::send_individual(const std::string &interface_name, const std::string &path,
                                    const AstarteData &data,
                                    const std::chrono::system_clock::time_point *timestamp) {
  astarte_device_impl_->send_individual(interface_name, path, data, timestamp);
}

void AstarteDevice::send_object(const std::string &interface_name, const std::string &path,
                                const AstarteDatastreamObject &object,
                                const std::chrono::system_clock::time_point *timestamp) {
  astarte_device_impl_->send_object(interface_name, path, object, timestamp);
}

void AstarteDevice::set_property(const std::string &interface_name, const std::string &path,
                                 const AstarteData &data) {
  astarte_device_impl_->set_property(interface_name, path, data);
}

void AstarteDevice::unset_property(const std::string &interface_name, const std::string &path) {
  astarte_device_impl_->unset_property(interface_name, path);
}

auto AstarteDevice::poll_incoming() -> std::optional<AstarteMessage> {
  return astarte_device_impl_->poll_incoming();
}

}  // namespace AstarteDeviceSdk
