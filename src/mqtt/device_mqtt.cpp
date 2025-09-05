// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/device_mqtt.hpp"

#include <chrono>
#include <filesystem>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"

namespace AstarteDeviceSdk {

AstarteDeviceMQTT::AstarteDeviceMQTT(const std::string& server_addr, const std::string& node_uuid)
// : astarte_device_impl_{std::make_shared<AstarteDeviceMQTTImpl>(server_addr, node_uuid)}
{}

AstarteDeviceMQTT::~AstarteDeviceMQTT() = default;

void AstarteDeviceMQTT::add_interface_from_file(const std::filesystem::path& json_file) {
  // astarte_device_impl_->add_interface_from_file(json_file);
}

void AstarteDeviceMQTT::add_interface_from_str(std::string_view json) {
  // astarte_device_impl_->add_interface_from_str(json);
}

void AstarteDeviceMQTT::remove_interface(const std::string& interface_name) {
  // astarte_device_impl_->remove_interface(interface_name);
}

void AstarteDeviceMQTT::connect() {
  // astarte_device_impl_->connect();
}

auto AstarteDeviceMQTT::is_connected() const -> bool {
  // return astarte_device_impl_->is_connected();
}

void AstarteDeviceMQTT::disconnect() {
  // astarte_device_impl_->disconnect();
}

void AstarteDeviceMQTT::send_individual(std::string_view interface_name, std::string_view path,
                                        const AstarteData& data,
                                        const std::chrono::system_clock::time_point* timestamp) {
  // astarte_device_impl_->send_individual(interface_name, path, data, timestamp);
}

void AstarteDeviceMQTT::send_object(std::string_view interface_name, std::string_view path,
                                    const AstarteDatastreamObject& object,
                                    const std::chrono::system_clock::time_point* timestamp) {
  // astarte_device_impl_->send_object(interface_name, path, object, timestamp);
}

void AstarteDeviceMQTT::set_property(std::string_view interface_name, std::string_view path,
                                     const AstarteData& data) {
  // astarte_device_impl_->set_property(interface_name, path, data);
}

void AstarteDeviceMQTT::unset_property(std::string_view interface_name, std::string_view path) {
  // astarte_device_impl_->unset_property(interface_name, path);
}

auto AstarteDeviceMQTT::poll_incoming(const std::chrono::milliseconds& timeout)
    -> std::optional<AstarteMessage> {
  // return astarte_device_impl_->poll_incoming(timeout);
}

auto AstarteDeviceMQTT::get_all_properties(const std::optional<AstarteOwnership>& ownership)
    -> std::list<AstarteStoredProperty> {
  // return astarte_device_impl_->get_all_properties(ownership);
}

auto AstarteDeviceMQTT::get_properties(std::string_view interface_name)
    -> std::list<AstarteStoredProperty> {
  // return astarte_device_impl_->get_properties(interface_name);
}

auto AstarteDeviceMQTT::get_property(std::string_view interface_name, std::string_view path)
    -> AstartePropertyIndividual {
  // return astarte_device_impl_->get_property(interface_name, path);
}

}  // namespace AstarteDeviceSdk
