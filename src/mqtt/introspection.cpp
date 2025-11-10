// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/introspection.hpp"

// Note: Other headers like <format>, <stdexcept>, etc., are included
// via "astarte_device_sdk/mqtt/introspection.hpp"

namespace AstarteDeviceSdk {

auto interface_type_from_str(std::string typ) -> InterfaceType {
  if (typ == "datastream") {
    return InterfaceType::kDatastream;
  } else if (typ == "properties") {
    return InterfaceType::kProperty;
  } else {
    throw InvalidInterfaceTypeException(std::format("interface type not valid: {}", typ));
  }
}

auto aggregation_from_str(std::string aggr) -> Aggregation {
  if (aggr == "individual") {
    return Aggregation::kIndividual;
  } else if (aggr == "object") {
    return Aggregation::kObject;
  } else {
    throw InvalidAggregationException(std::format("Aggregation not valid: {}", aggr));
  }
}

auto mappings_from_interface(json& interface) -> std::vector<Mapping> {
  std::vector<Mapping> mappings;

  for (const auto& mapping : interface.at("mappings")) {
    auto endpoint = mapping.at("endpoint");
    auto mapping_type = astarte_type_from_str(mapping.at("type"));
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

    auto m = Mapping{endpoint,
                     mapping_type,
                     explicit_timestamp,
                     reliability,
                     retention,
                     expiry,
                     database_retention_policy,
                     database_retention_ttl,
                     allow_unset,
                     description,
                     doc};

    mappings.push_back(m);
  }

  return mappings;
}

auto convert_version(std::string_view maj_min, int64_t version) -> u_int32_t {
  if (std::cmp_less(version, 0)) {
    throw InvalidVersionException(
        std::format("received negative {} version value: {}", maj_min, version));
  }

  if (std::cmp_greater(version, std::numeric_limits<u_int32_t>::max())) {
    throw InvalidVersionException(std::format("{} version value too large: {}", maj_min, version));
  }

  return static_cast<u_int32_t>(version);
}

auto Interface::try_from_json(json interface) -> Interface {
  auto interface_name = interface.at("interface_name");
  auto version_major = convert_version("major", interface.at("version_major"));
  auto version_minor = convert_version("minor", interface.at("version_minor"));
  auto interface_type = interface_type_from_str(interface.at("type"));
  auto ownership = ownership_from_str(interface.at("ownership"));
  auto aggregation = [&interface]() {
    return interface.contains("aggregation")
               ? std::optional(aggregation_from_str(interface.at("aggregation")))
               : std::nullopt;
  }();
  auto description = optional_value_from_json_interface<std::string>(interface, "description");
  auto doc = optional_value_from_json_interface<std::string>(interface, "doc");

  auto mappings = mappings_from_interface(interface);

  return Interface{interface_name, version_major, version_minor, interface_type, ownership,
                   aggregation,    description,   doc,           mappings};
}

}  // namespace AstarteDeviceSdk
