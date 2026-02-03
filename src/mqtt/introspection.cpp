// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/introspection.hpp"

#include <spdlog/spdlog.h>

#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

auto Introspection::checked_insert(Interface interface) -> astarte_tl::expected<void, Error> {
  const std::unique_lock lock(lock_);

  auto iter = interfaces_.find(interface.interface_name());
  if (iter == interfaces_.end()) {
    spdlog::debug("Adding new interface {}", interface.interface_name());
    // Move the interface into a shared_ptr<const Interface>
    const std::string name = interface.interface_name();
    interfaces_.emplace(name, std::make_shared<const Interface>(std::move(interface)));
    return {};
  }

  // Dereference the shared_ptr to get the stored const Interface&
  const auto& stored = *(iter->second);

  if (stored.ownership() != interface.ownership()) {
    spdlog::error("the new interface has a different ownership");
    return astarte_tl::unexpected(InvalidInterfaceOwnershipeError(
        astarte_fmt::format("the new interface has a different ownership. Expected {}, got {}",
                            stored.ownership(), interface.ownership())));
  }

  if (stored.interface_type() != interface.interface_type()) {
    spdlog::error("the new interface has a different type");
    return astarte_tl::unexpected(InvalidAstarteTypeError(
        astarte_fmt::format("the new interface has a different type. Expected {}, got {}",
                            stored.interface_type(), interface.interface_type())));
  }

  if (interface.version_major() < stored.version_major()) {
    spdlog::error("the new interface must have a major version greater or equal than {}",
                  stored.version_major());
    return astarte_tl::unexpected(InvalidInterfaceVersionError(
        astarte_fmt::format("the new major version is lower than the actual one. Expected value "
                            "greater than {}, got {}",
                            stored.version_major(), interface.version_major())));
  }

  if ((interface.version_major() == stored.version_major()) &&
      (interface.version_minor() < stored.version_minor())) {
    spdlog::error("the new interface must have a minor version greater or equal than {}",
                  stored.version_minor());
    return astarte_tl::unexpected(InvalidInterfaceVersionError(astarte_fmt::format(
        "the new minor version is lower than the actual one Expected value greater than {}, got {}",
        stored.version_minor(), interface.version_minor())));
  }

  spdlog::debug("overwriting the old interface with the new one");
  const std::string name = interface.interface_name();
  interfaces_.insert_or_assign(name, std::make_shared<const Interface>(std::move(interface)));
  return {};
}

auto Introspection::values() const -> std::vector<std::shared_ptr<const Interface>> {
  const std::shared_lock lock(lock_);

  std::vector<std::shared_ptr<const Interface>> result;
  result.reserve(interfaces_.size());
  for (const auto& key_value : interfaces_) {
    result.push_back(key_value.second);
  }

  return result;
}

auto Introspection::get(std::string_view interface_name) const
    -> astarte_tl::expected<std::shared_ptr<const Interface>, Error> {
  const std::shared_lock lock(lock_);

  auto iter = interfaces_.find(interface_name);
  if (iter == interfaces_.end()) {
    return astarte_tl::unexpected(MqttError(
        astarte_fmt::format("couldn't find interface {} in the introspection", interface_name)));
  }

  return iter->second;
}

}  // namespace astarte::device
