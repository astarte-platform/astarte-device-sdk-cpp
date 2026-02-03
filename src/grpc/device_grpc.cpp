// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/grpc/device_grpc.hpp"

#include <chrono>
#include <filesystem>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#if defined(ASTARTE_USE_TL_EXPECTED)
#include "tl/expected.hpp"
#else
#include <expected>
#endif

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "grpc/device_grpc_impl.hpp"

namespace astarte::device {

DeviceGrpc::DeviceGrpc(const std::string& server_addr, const std::string& node_uuid)
    : astarte_device_impl_{std::make_shared<AstarteDeviceGrpcImpl>(server_addr, node_uuid)} {}

DeviceGrpc::~DeviceGrpc() = default;

auto DeviceGrpc::add_interface_from_file(const std::filesystem::path& json_file)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->add_interface_from_file(json_file);
}

auto DeviceGrpc::add_interface_from_str(std::string_view json)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->add_interface_from_str(json);
}

auto DeviceGrpc::remove_interface(const std::string& interface_name)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->remove_interface(interface_name);
}

auto DeviceGrpc::connect() -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->connect();
}

auto DeviceGrpc::is_connected() const -> bool { return astarte_device_impl_->is_connected(); }

auto DeviceGrpc::disconnect() -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->disconnect();
}

auto DeviceGrpc::send_individual(std::string_view interface_name, std::string_view path,
                                 const Data& data,
                                 const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->send_individual(interface_name, path, data, timestamp);
}

auto DeviceGrpc::send_object(std::string_view interface_name, std::string_view path,
                             const DatastreamObject& object,
                             const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->send_object(interface_name, path, object, timestamp);
}

auto DeviceGrpc::set_property(std::string_view interface_name, std::string_view path,
                              const Data& data) -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->set_property(interface_name, path, data);
}

auto DeviceGrpc::unset_property(std::string_view interface_name, std::string_view path)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->unset_property(interface_name, path);
}

auto DeviceGrpc::poll_incoming(const std::chrono::milliseconds& timeout) -> std::optional<Message> {
  return astarte_device_impl_->poll_incoming(timeout);
}

auto DeviceGrpc::get_all_properties(const std::optional<Ownership>& ownership)
    -> astarte_tl::expected<std::list<StoredProperty>, Error> {
  return astarte_device_impl_->get_all_properties(ownership);
}

auto DeviceGrpc::get_properties(std::string_view interface_name)
    -> astarte_tl::expected<std::list<StoredProperty>, Error> {
  return astarte_device_impl_->get_properties(interface_name);
}

auto DeviceGrpc::get_property(std::string_view interface_name, std::string_view path)
    -> astarte_tl::expected<PropertyIndividual, Error> {
  return astarte_device_impl_->get_property(interface_name, path);
}

}  // namespace astarte::device
