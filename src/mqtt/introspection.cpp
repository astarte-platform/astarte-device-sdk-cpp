// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/introspection.hpp"

namespace AstarteDeviceSdk {

auto Introspection::checked_insert(Interface interface)
    -> astarte_tl::expected<void, AstarteError> {
  if (!interfaces_.contains(interface.interface_name())) {
    spdlog::debug("Adding new interface {}", interface.interface_name());
    interfaces_.insert_or_assign(interface.interface_name(), std::move(interface));
    return {};
  }

  // if the interface is already present in the introspection, do some checks before updating it
  const auto& stored = interfaces_.at(interface.interface_name());
  if (stored.ownership() != interface.ownership()) {
    spdlog::error("the new interface has a different ownership");
    return astarte_tl::unexpected(AstarteInvalidInterfaceOwnershipeError(
        astarte_fmt::format("the new interface has a different ownership. Expected {}, got {}",
                            stored.ownership(), interface.ownership())));
  }

  if (stored.interface_type() != interface.interface_type()) {
    spdlog::error("the new interface has a different type");
    return astarte_tl::unexpected(AstarteInvalidAstarteTypeError(
        astarte_fmt::format("the new interface has a different type. Expected {}, got {}",
                            stored.interface_type(), interface.interface_type())));
  }

  if (interface.version_major() < stored.version_major()) {
    spdlog::error("the new interface must have a major version greater or equal than {}",
                  stored.version_major());
    return astarte_tl::unexpected(AstarteInvalidInterfaceVersionError(
        astarte_fmt::format("the new major version is lower than the actual one. Expected value "
                            "greater than {}, got {}",
                            stored.version_major(), interface.version_major())));
  }

  if ((interface.version_major() == stored.version_major()) &&
      (interface.version_minor() < stored.version_minor())) {
    spdlog::error("the new interface must have a minor version greater or equal than {}",
                  stored.version_minor());
    return astarte_tl::unexpected(AstarteInvalidInterfaceVersionError(astarte_fmt::format(
        "the new minor version is lower than the actual one Expected value greater than {}, got {}",
        stored.version_minor(), interface.version_minor())));
  }

  spdlog::debug("overwriting the old interface with the new one");
  interfaces_.insert_or_assign(interface.interface_name(), std::move(interface));
  return {};
}

auto Introspection::get(const std::string& interface_name)
    -> astarte_tl::expected<Interface*, AstarteError> {
  if (!interfaces_.contains(interface_name)) {
    return astarte_tl::unexpected(AstarteMqttError(
        astarte_fmt::format("couldn't find interface {} in the introspection", interface_name)));
  }

  return &interfaces_.at(interface_name);
}

}  // namespace AstarteDeviceSdk
