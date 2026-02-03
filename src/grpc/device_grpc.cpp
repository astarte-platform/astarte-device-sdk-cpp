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

AstarteDeviceGrpc::AstarteDeviceGrpc(const std::string& server_addr, const std::string& node_uuid)
    : astarte_device_impl_{std::make_shared<AstarteDeviceGrpcImpl>(server_addr, node_uuid)} {}

AstarteDeviceGrpc::~AstarteDeviceGrpc() = default;

auto AstarteDeviceGrpc::add_interface_from_file(const std::filesystem::path& json_file)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->add_interface_from_file(json_file);
}

auto AstarteDeviceGrpc::add_interface_from_str(std::string_view json)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->add_interface_from_str(json);
}

auto AstarteDeviceGrpc::remove_interface(const std::string& interface_name)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->remove_interface(interface_name);
}

auto AstarteDeviceGrpc::connect() -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->connect();
}

auto AstarteDeviceGrpc::is_connected() const -> bool {
  return astarte_device_impl_->is_connected();
}

auto AstarteDeviceGrpc::disconnect() -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->disconnect();
}

auto AstarteDeviceGrpc::send_individual(std::string_view interface_name, std::string_view path,
                                        const Data& data,
                                        const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->send_individual(interface_name, path, data, timestamp);
}

auto AstarteDeviceGrpc::send_object(std::string_view interface_name, std::string_view path,
                                    const AstarteDatastreamObject& object,
                                    const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->send_object(interface_name, path, object, timestamp);
}

auto AstarteDeviceGrpc::set_property(std::string_view interface_name, std::string_view path,
                                     const Data& data) -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->set_property(interface_name, path, data);
}

auto AstarteDeviceGrpc::unset_property(std::string_view interface_name, std::string_view path)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->unset_property(interface_name, path);
}

auto AstarteDeviceGrpc::poll_incoming(const std::chrono::milliseconds& timeout)
    -> std::optional<AstarteMessage> {
  return astarte_device_impl_->poll_incoming(timeout);
}

auto AstarteDeviceGrpc::get_all_properties(const std::optional<AstarteOwnership>& ownership)
    -> astarte_tl::expected<std::list<AstarteStoredProperty>, Error> {
  return astarte_device_impl_->get_all_properties(ownership);
}

auto AstarteDeviceGrpc::get_properties(std::string_view interface_name)
    -> astarte_tl::expected<std::list<AstarteStoredProperty>, Error> {
  return astarte_device_impl_->get_properties(interface_name);
}

auto AstarteDeviceGrpc::get_property(std::string_view interface_name, std::string_view path)
    -> astarte_tl::expected<AstartePropertyIndividual, Error> {
  return astarte_device_impl_->get_property(interface_name, path);
}

}  // namespace astarte::device
