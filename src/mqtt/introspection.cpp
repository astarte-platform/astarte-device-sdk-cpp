// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/introspection.hpp"

#include <spdlog/spdlog.h>

#include <cstdint>
#include <format>
#include <limits>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/type.hpp"

namespace AstarteDeviceSdk {

auto interface_type_from_str(std::string typ) -> astarte_tl::expected<InterfaceType, AstarteError> {
  if (typ == "datastream") {
    return InterfaceType::kDatastream;
  }
  if (typ == "properties") {
    return InterfaceType::kProperty;
  }

  return astarte_tl::unexpected(
      AstarteInvalidInterfaceTypeError(astarte_fmt::format("interface type not valid: {}", typ)));
}

inline auto aggregation_from_str(std::string aggr)
    -> astarte_tl::expected<Aggregation, AstarteError> {
  if (aggr == "individual") {
    return Aggregation::kIndividual;
  }
  if (aggr == "object") {
    return Aggregation::kObject;
  }

  return astarte_tl::unexpected(AstarteInvalidInterfaceAggregationError(
      astarte_fmt::format("Aggregation not valid: {}", aggr)));
}

auto mappings_from_interface(const json& interface)
    -> astarte_tl::expected<std::vector<Mapping>, AstarteError> {
  std::vector<Mapping> mappings;

  for (const auto& mapping : interface.at("mappings")) {
    auto endpoint = mapping.at("endpoint");
    auto mapping_type = astarte_type_from_str(mapping.at("type"));
    if (!mapping_type) {
      return astarte_tl::unexpected(mapping_type.error());
    }
    auto explicit_timestamp =
        optional_value_from_json_interface<bool>(mapping, "explicit_timestamp");
    auto reliability = optional_value_from_json_interface<Reliability>(mapping, "reliability");
    auto retention = optional_value_from_json_interface<Retention>(mapping, "retention");
    auto expiry = optional_value_from_json_interface<int64_t>(mapping, "expiry");
    auto database_retention_policy = optional_value_from_json_interface<DatabaseRetentionPolicy>(
        mapping, "database_retention_policy");
    auto database_retention_ttl =
        optional_value_from_json_interface<int64_t>(mapping, "database_retention_ttl");
    auto allow_unset = optional_value_from_json_interface<bool>(mapping, "allow_unset");
    auto description = optional_value_from_json_interface<std::string>(mapping, "description");
    auto doc = optional_value_from_json_interface<std::string>(mapping, "doc");

    mappings.emplace_back(Mapping{.endpoint = endpoint,
                                  .mapping_type = mapping_type.value(),
                                  .explicit_timestamp = explicit_timestamp,
                                  .reliability = reliability,
                                  .retention = retention,
                                  .expiry = expiry,
                                  .database_retention_policy = database_retention_policy,
                                  .database_retention_ttl = database_retention_ttl,
                                  .allow_unset = allow_unset,
                                  .description = description,
                                  .doc = doc});
  }

  return mappings;
}

auto convert_version(std::string_view version_type, int64_t version)
    -> astarte_tl::expected<uint32_t, AstarteError> {
  if (std::cmp_less(version, 0)) {
    return astarte_tl::unexpected(AstarteInvalidInterfaceVersionError(
        astarte_fmt::format("received negative {} version value: {}", version_type, version)));
  }

  if (std::cmp_greater(version, std::numeric_limits<uint32_t>::max())) {
    return astarte_tl::unexpected(AstarteInvalidInterfaceVersionError(
        astarte_fmt::format("{} version value too large: {}", version_type, version)));
  }

  return static_cast<uint32_t>(version);
}

// NOLINTNEXTLINE(readability-function-size)
auto Interface::try_from_json(const json& interface)
    -> astarte_tl::expected<Interface, AstarteError> {
  const auto& interface_name = interface.at("interface_name");
  auto version_major = convert_version("major", interface.at("version_major"));
  if (!version_major) {
    return astarte_tl::unexpected(version_major.error());
  }
  auto version_minor = convert_version("minor", interface.at("version_minor"));
  if (!version_minor) {
    return astarte_tl::unexpected(version_minor.error());
  }
  auto interface_type = interface_type_from_str(interface.at("type"));
  if (!interface_type) {
    return astarte_tl::unexpected(interface_type.error());
  }
  auto ownership = ownership_from_str(interface.at("ownership"));
  if (!ownership) {
    return astarte_tl::unexpected(ownership.error());
  }
  std::optional<Aggregation> aggregation = std::nullopt;
  auto contains_aggr = interface.contains("aggregation");
  if (contains_aggr) {
    auto res = aggregation_from_str(interface.at("aggregation"));
    if (!res) {
      return astarte_tl::unexpected(res.error());
    }
    aggregation = std::optional(res.value());
  }

  auto description = optional_value_from_json_interface<std::string>(interface, "description");
  auto doc = optional_value_from_json_interface<std::string>(interface, "doc");

  auto mappings = mappings_from_interface(interface);
  if (!mappings) {
    return astarte_tl::unexpected(mappings.error());
  }

  return Interface(interface_name, version_major.value(), version_minor.value(),
                   interface_type.value(), ownership.value(), aggregation, description, doc,
                   mappings.value());
}

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

}  // namespace AstarteDeviceSdk
