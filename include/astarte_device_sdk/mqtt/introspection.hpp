// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_INTROSPECTION_H
#define ASTARTE_MQTT_INTROSPECTION_H

#include <spdlog/spdlog.h>

#include <astarte_device_sdk/exceptions.hpp>
#include <astarte_device_sdk/ownership.hpp>
#include <astarte_device_sdk/type.hpp>
#include <format>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace AstarteDeviceSdk {

using json = nlohmann::json;

// TODO: remove inline once the function definition is moved to its respective .cpp file
template <typename T>
inline auto optional_value_from_json_interface(const json& interface, std::string_view key)
    -> std::optional<T> {
  return interface.contains(key) ? std::optional<T>(interface.at(key)) : std::nullopt;
}

enum InterfaceType {
  kDatastream,
  kProperty,
};

// TODO: remove inline once the function definition is moved to its respective .cpp file
inline auto interface_type_from_str(std::string typ) -> InterfaceType {
  if (typ == "datastream") {
    return InterfaceType::kDatastream;
  } else if (typ == "properties") {
    return InterfaceType::kProperty;
  } else {
    throw InvalidInterfaceTypeException(std::format("interface type not valid: {}", typ));
  }
}

enum Aggregation {
  kIndividual,
  kObject,
};

// TODO: remove inline once the function definition is moved to its respective .cpp file
inline auto aggregation_from_str(std::string aggr) -> Aggregation {
  if (aggr == "individual") {
    return Aggregation::kIndividual;
  } else if (aggr == "object") {
    return Aggregation::kObject;
  } else {
    throw InvalidAggregationException(std::format("Aggregation not valid: {}", aggr));
  }
}

/// Reliability of a data stream.
///
/// Defines whether the sent data should be considered delivered.
///
/// Properties have always a unique reliability.
///
/// See
/// [Reliability](https://docs.astarte-platform.org/astarte/latest/040-interface_schema.html#astarte-mapping-schema-reliability)
/// for more information.
enum Reliability {
  /// If the transport sends the data. It is the default value. Default.
  kUnreliable,
  /// When we know the data has been received at least once.
  kGuaranteed,
  /// When we know the data has been received exactly once.
  kUnique,
};

NLOHMANN_JSON_SERIALIZE_ENUM(Reliability, {
                                              {kUnreliable, "unreliable"},
                                              {kGuaranteed, "guaranteed"},
                                              {kUnique, "unique"},
                                          })

/// Defines the retention of a data stream.
///
/// Describes what to do with the sent data if the transport is incapable of delivering it.
///
/// See
/// [Retention](https://docs.astarte-platform.org/astarte/latest/040-interface_schema.html#astarte-mapping-schema-retention)
/// for more information.
enum Retention {
  /// Data is discarded. Default.
  kDiscard,
  /// Data is kept in a cache in memory.
  kVolatile,
  /// Data is kept on disk.
  kStored,
};

NLOHMANN_JSON_SERIALIZE_ENUM(Retention, {
                                            {kDiscard, "discard"},
                                            {kVolatile, "volatile"},
                                            {kStored, "stored"},
                                        })

/// Defines whether data should expire from the database after a given interval.
///
/// See
/// [Database Retention
/// Policy](https://docs.astarte-platform.org/astarte/latest/040-interface_schema.html#astarte-mapping-schema-database_retention_policy)
/// for more information.
enum DatabaseRetentionPolicy {
  /// The data will never expiry. Default.
  kNoTtl,
  /// The data will expire after the ttl.
  ///
  /// The field [`database_retention_ttl`](Mapping::database_retention_ttl) will be used to
  /// determine how many seconds the data is kept in the database.
  kUseTtl,
};

NLOHMANN_JSON_SERIALIZE_ENUM(DatabaseRetentionPolicy, {
                                                          {kNoTtl, "no_ttl"},
                                                          {kUseTtl, "use_ttl"},
                                                      })

struct Mapping {
 public:
  /// Path of the mapping.
  ///
  /// It can be parametrized (e.g. `/foo/%{path}/baz`).
  std::string endpoint;
  /// Defines the type of the mapping.
  ///
  /// This represent the data that will be published on the mapping.
  AstarteType mapping_type;
  /// Allow to set a custom timestamp.
  std::optional<bool> explicit_timestamp;
  /// Defines when to consider the data delivered.
  ///
  /// Useful only with datastream. Defines whether the sent data should be considered delivered
  /// when the transport successfully sends the data (unreliable), when we know that the data has
  /// been received at least once (guaranteed) or when we know that the data has been received
  /// exactly once (unique). Unreliable by default.
  std::optional<Reliability> reliability;
  /// Retention of the data when not deliverable.
  ///
  /// Useful only with datastream. Defines whether the sent data should be discarded if the
  /// transport is temporarily uncapable of delivering it (discard) or should be kept in a cache in
  /// memory (volatile) or on disk (stored), and guaranteed to be delivered in the timeframe
  /// defined by the expiry.
  std::optional<Retention> retention;
  /// Expiry for the retain data.
  ///
  /// Useful when retention is stored. Defines after how many seconds a specific data entry should
  /// be kept before giving up and erasing it from the persistent cache. A value <= 0 means the
  /// persistent cache never expires, and is the default.
  std::optional<int64_t> expiry;
  /// Expiry for the retain data.
  ///
  /// Retention policy for the database.
  ///
  /// Useful only with datastream. Defines whether data should expire from the database after a
  /// given interval. Valid values are: `no_ttl` and `use_ttl`.
  std::optional<DatabaseRetentionPolicy> database_retention_policy;
  /// Seconds to keep the data in the database.
  ///
  /// Useful when `database_retention_policy` is "`use_ttl`". Defines how many seconds a specific
  /// data entry should be kept before erasing it from the database.
  std::optional<int64_t> database_retention_ttl;
  /// Allows the property to be unset.
  ///
  /// Used only with properties.
  std::optional<bool> allow_unset;
  /// An optional description of the mapping.
  std::optional<std::string> description;
  /// A string containing documentation that will be injected in the generated client code.
  std::optional<std::string> doc;
};

// TODO: remove inline once the function definition is moved to its respective .cpp file
inline auto mappings_from_interface(json& interface) -> std::vector<Mapping> {
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

// TODO: remove inline once the function definition is moved to its respective .cpp file
inline auto convert_version(std::string_view maj_min, int64_t version) -> u_int32_t {
  if (std::cmp_less(version, 0)) {
    throw InvalidVersionException(
        std::format("received negative {} version value: {}", maj_min, version));
  }

  if (std::cmp_greater(version, std::numeric_limits<u_int32_t>::max())) {
    throw InvalidVersionException(std::format("{} version value too large: {}", maj_min, version));
  }

  return static_cast<u_int32_t>(version);
}

class Interface {
 public:
  static auto try_from_json(json interface) -> Interface {
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

  /// The name of the interface.
  std::string interface_name;
  /// The Major version qualifier for this interface.
  u_int32_t version_major;
  /// The Minor version qualifier for this interface.
  u_int32_t version_minor;
  /// Identifies the type of this Interface. It could be Datastream or Property.
  InterfaceType interface_type;
  /// Identifies the quality of the interface.
  AstarteOwnership ownership;
  /// Identifies the aggregation of the mappings of the interface. It could be Individual or
  // Object.
  std::optional<Aggregation> aggregation;
  /// An optional description of the interface.
  std::optional<std::string> description;
  /// A string containing documentation that will be injected in the generated client code.
  std::optional<std::string> doc;
  /// Mappings define the endpoint of the interface, where actual data is stored/streamed.
  ///
  /// They are defined as relative URLs (e.g. /my/path) and can be parametrized (e.g.:
  /// /%{myparam}/path). A valid interface must have no mappings clash, which means that every
  /// mapping must resolve to a unique path or collection of paths (including
  /// parametrization). Every mapping acquires type, quality and aggregation of the interface.
  std::vector<Mapping> mappings;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_INTROSPECTION_H
