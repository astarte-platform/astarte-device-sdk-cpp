// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/introspection.hpp"

// Note: Other headers like <format>, <stdexcept>, etc., are included
// via "astarte_device_sdk/mqtt/introspection.hpp"

namespace AstarteDeviceSdk {

auto interface_type_from_str(std::string typ) -> astarte_tl::expected<InterfaceType, AstarteError> {
  if (typ == "datastream") {
    return InterfaceType::kDatastream;
  } else if (typ == "properties") {
    return InterfaceType::kProperty;
  } else {
    return astarte_tl::unexpected(
        AstarteInvalidInterfaceTypeError(std::format("interface type not valid: {}", typ)));
  }
}

inline auto aggregation_from_str(std::string aggr)
    -> astarte_tl::expected<Aggregation, AstarteError> {
  if (aggr == "individual") {
    return Aggregation::kIndividual;
  } else if (aggr == "object") {
    return Aggregation::kObject;
  } else {
    return astarte_tl::unexpected(
        AstarteInvalidAggregationError(std::format("Aggregation not valid: {}", aggr)));
  }
}

auto mappings_from_interface(json& interface)
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

    auto m = Mapping{
        endpoint, mapping_type.value(),      explicit_timestamp,     reliability, retention,
        expiry,   database_retention_policy, database_retention_ttl, allow_unset, description,
        doc};

    mappings.push_back(m);
  }

  return mappings;
}

auto convert_version(std::string_view maj_min, int64_t version)
    -> astarte_tl::expected<u_int32_t, AstarteError> {
  if (std::cmp_less(version, 0)) {
    return astarte_tl::unexpected(AstarteInvalidVersionError(
        std::format("received negative {} version value: {}", maj_min, version)));
  }

  if (std::cmp_greater(version, std::numeric_limits<u_int32_t>::max())) {
    return astarte_tl::unexpected(AstarteInvalidVersionError(
        std::format("{} version value too large: {}", maj_min, version)));
  }

  return static_cast<u_int32_t>(version);
}

auto Interface::try_from_json(json interface) -> astarte_tl::expected<Interface, AstarteError> {
  auto interface_name = interface.at("interface_name");
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

  return Interface{
      interface_name,    version_major.value(), version_minor.value(), interface_type.value(),
      ownership.value(), aggregation,           description,           doc,
      mappings.value()};
}

}  // namespace AstarteDeviceSdk
