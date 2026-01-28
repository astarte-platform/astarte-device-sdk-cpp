// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_INTROSPECTION_H
#define ASTARTE_MQTT_INTROSPECTION_H

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <format>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/type.hpp"

namespace AstarteDeviceSdk {

/**
 * @brief alias for nlohmann json
 */
using json = nlohmann::json;

/**
 * @brief Define the type of an Astarte interface.
 */
enum InterfaceType : uint8_t {
  /**
   * @brief A datastream interface, used for time-series data.
   */
  kDatastream,
  /**
   * @brief A properties interface, used for device state properties.
   */
  kProperty,
};

/**
 * @brief Convert a string to an InterfaceType enum.
 *
 * @param typ The string representation of the interface type.
 * @return The corresponding InterfaceType enum value, an error if the string is not a valid
 * interface type.
 */
auto interface_type_from_str(std::string typ) -> astarte_tl::expected<InterfaceType, AstarteError>;

/**
 * @brief Define the aggregation type for interface mappings.
 */
enum InterfaceAggregation : uint8_t {
  /**
   * @brief Data is collected as individual, distinct values.
   */
  kIndividual,
  /**
   * @brief Data is collected as a single object or document.
   */
  kObject,
};

/**
 * @brief Convert a string to an InterfaceAggregation enum.
 *
 * @param aggr The string representation of the aggregation (e.g., "individual", "object").
 * @return The corresponding InterfaceAggregation enum value, an error if the string is not a valid
 * aggregation type.
 */
auto aggregation_from_str(std::string aggr)
    -> astarte_tl::expected<InterfaceAggregation, AstarteError>;

/**
 * @brief Reliability of a datastream.
 *
 * Defines whether the sent data should be considered delivered. Properties have always a unique
 * reliability. See
 * [Reliability](https://docs.astarte-platform.org/astarte/latest/040-interface_schema.html#astarte-mapping-schema-reliability)
 * for more information.
 */
enum Reliability : uint8_t {
  /**
   * @brief If the transport sends the data. Default.
   */
  kUnreliable,
  /**
   * @brief When we know the data has been received at least once.
   */
  kGuaranteed,
  /**
   * @brief When we know the data has been received exactly once.
   */
  kUnique,
};

NLOHMANN_JSON_SERIALIZE_ENUM(Reliability, {
                                              {kUnreliable, "unreliable"},
                                              {kGuaranteed, "guaranteed"},
                                              {kUnique, "unique"},
                                          })

/**
 * @brief Define the retention of a datastream.
 *
 * Describes what to do with the sent data if the transport is incapable of delivering it.
 * See
 * [Retention](https://docs.astarte-platform.org/astarte/latest/040-interface_schema.html#astarte-mapping-schema-retention)
 * for more information.
 */
enum Retention : uint8_t {
  /**
   * @brief Data is discarded. Default.
   */
  kDiscard,
  /**
   * @brief Data is kept in a cache in memory.
   */
  kVolatile,
  /**
   * @brief Data is kept on non-volatile storage.
   */
  kStored,
};

NLOHMANN_JSON_SERIALIZE_ENUM(Retention, {
                                            {kDiscard, "discard"},
                                            {kVolatile, "volatile"},
                                            {kStored, "stored"},
                                        })

/**
 * @brief Define whether data should expire from the database after a given interval.
 *
 * See [Database Retention
 * Policy](https://docs.astarte-platform.org/astarte/latest/040-interface_schema.html#astarte-mapping-schema-database_retention_policy)
 * for more information.
 */
enum DatabaseRetentionPolicy : uint8_t {
  /**
   * @brief The data will never expiry. Default.
   */
  kNoTtl,
  /**
   * @brief The data will expire after the ttl.
   *
   * The field `database_retention_ttl` will be used to determine how many seconds the data is kept
   * in the database.
   */
  kUseTtl,
};

NLOHMANN_JSON_SERIALIZE_ENUM(DatabaseRetentionPolicy, {
                                                          {kNoTtl, "no_ttl"},
                                                          {kUseTtl, "use_ttl"},
                                                      })

class Mapping {
 public:
  Mapping(std::string endpoint, AstarteType type, std::optional<bool> explicit_timestamp,
          std::optional<Reliability> reliability, std::optional<Retention> retention,
          std::optional<int64_t> expiry,
          std::optional<DatabaseRetentionPolicy> database_retention_policy,
          std::optional<int64_t> database_retention_ttl, std::optional<bool> allow_unset,
          std::optional<std::string> description, std::optional<std::string> doc)
      : endpoint_(std::move(endpoint)),
        type_(type),
        explicit_timestamp_(explicit_timestamp),
        reliability_(reliability),
        retention_(retention),
        expiry_(expiry),
        database_retention_policy_(database_retention_policy),
        database_retention_ttl_(database_retention_ttl),
        allow_unset_(allow_unset),
        description_(std::move(description)),
        doc_(std::move(doc)) {}

  /**
   * @brief Check that the mapping endpoint matches a given path.
   *
   * @param path The Astarte interface path to check.
   * @return a boolean stating if the mapping endpoint matches the path or not.
   */
  [[nodiscard]] auto match_path(std::string_view path) const -> bool;

  /**
   * @brief Check that the Astarte data matches the mapping type
   *
   * @param data The AstarteData to check.
   * @return an error if the check fails.
   */
  [[nodiscard]] auto check_data_type(const AstarteData& data) const
      -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Path of the mapping.
   *
   * It can be parametrized (e.g. `/foo/%{path}/baz`).
   */
  [[nodiscard]] auto endpoint() const -> const std::string& { return endpoint_; }

  /**
   * @brief Define the type of the mapping.
   *
   * This represent the data that will be published on the mapping.
   */
  [[nodiscard]] auto type() const -> AstarteType { return type_; }

  /**
   * @brief Allow to set a custom timestamp.
   */
  [[nodiscard]] auto explicit_timestamp() const -> std::optional<bool> {
    return explicit_timestamp_;
  }

  /**
   * @brief Define when to consider the data delivered.
   *
   * Useful only with datastream. Defines whether the sent data should be considered delivered
   * when the transport successfully sends the data (unreliable), when we know that the data has
   * been received at least once (guaranteed) or when we know that the data has been received
   * exactly once (unique). Unreliable by default.
   */
  [[nodiscard]] auto reliability() const -> std::optional<Reliability> { return reliability_; }

  /**
   * @brief Retention of the data when not deliverable.
   *
   * Useful only with datastream. Defines whether the sent data should be discarded if the
   * transport is temporarily uncapable of delivering it (discard) or should be kept in a cache in
   * memory (volatile) or on disk (stored), and guaranteed to be delivered in the timeframe
   * defined by the expiry.
   */
  [[nodiscard]] auto retention() const -> std::optional<Retention> { return retention_; }

  /**
   * @brief Expiry for the retain data.
   *
   * Useful when retention is stored. Defines after how many seconds a specific data entry should
   * be kept before giving up and erasing it from the persistent cache. A value <= 0 means the
   * persistent cache never expires, and is the default.
   */
  [[nodiscard]] auto expiry() const -> std::optional<int64_t> { return expiry_; }

  /**
   * @brief Expiry for the retain data.
   *
   * Useful only with datastream. Defines whether data should expire from the database after a
   * given interval. Valid values are: `no_ttl` and `use_ttl`.
   */
  [[nodiscard]] auto database_retention_policy() const -> std::optional<DatabaseRetentionPolicy> {
    return database_retention_policy_;
  }

  /**
   * @brief Seconds to keep the data in the database.
   *
   * Useful when `database_retention_policy` is "`use_ttl`". Defines how many seconds a specific
   * data entry should be kept before erasing it from the database.
   */
  [[nodiscard]] auto database_retention_ttl() const -> std::optional<int64_t> {
    return database_retention_ttl_;
  }

  /**
   * @brief Allow the property to be unset.
   *
   * Used only with properties.
   */
  [[nodiscard]] auto allow_unset() const -> std::optional<bool> { return allow_unset_; }

  /**
   * @brief An optional description of the mapping.
   */
  [[nodiscard]] auto description() const -> const std::optional<std::string>& {
    return description_;
  }

  /**
   * @brief A string containing documentation that will be injected in the generated client code.
   */
  [[nodiscard]] auto doc() const -> const std::optional<std::string>& { return doc_; }

 private:
  std::string endpoint_;
  AstarteType type_;
  std::optional<bool> explicit_timestamp_;
  std::optional<Reliability> reliability_;
  std::optional<Retention> retention_;
  std::optional<int64_t> expiry_;
  std::optional<DatabaseRetentionPolicy> database_retention_policy_;
  std::optional<int64_t> database_retention_ttl_;
  std::optional<bool> allow_unset_;
  std::optional<std::string> description_;
  std::optional<std::string> doc_;
};

/**
 * @brief Parses the "mappings" array from an interface JSON object.
 *
 * @param interface The JSON object representing an Astarte interface.
 * @return A vector of Mapping objects parsed from the interface, an error otherwise.
 */
auto mappings_from_interface_json(const json& interface)
    -> astarte_tl::expected<std::vector<Mapping>, AstarteError>;

/**
 * @brief Converts and validates an interface version number.
 *
 * Checks if the version is non-negative and fits within a uint32_t.
 *
 * @param version_type A string literal ("major" or "minor") used for error messages.
 * @param version The version number as int64_t (from JSON parsing).
 * @return The version number as uint32_t, an error if the version is negative or too large.
 */

auto convert_version(std::string_view version_type, int64_t version)
    -> astarte_tl::expected<uint32_t, AstarteError>;

/**
 * @brief Represents a parsed Astarte interface.
 */
class Interface {
 public:
  /**
   * @brief Try to convert a json into an Interface object.
   * @param interface json representation of the Astarte interface.
   * @return An Interface object containg all the parsed Astarte interface information.
   */
  static auto try_from_json(const json& interface) -> astarte_tl::expected<Interface, AstarteError>;

  /**
   * @brief Move constructor.
   *
   * @param other The Interface object to move from.
   */
  Interface(Interface&& other) noexcept = default;

  /**
   * @brief Move assignment operator.
   *
   * @param other The Interface object to move from.
   * @return A reference to this Interface object.
   */
  auto operator=(Interface&& other) noexcept -> Interface& = default;

  /**
   * @brief Deleted copy constructor.
   */
  Interface(const Interface&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   */
  auto operator=(const Interface&) -> Interface& = delete;

  /**
   * @brief Destructor.
   */
  ~Interface() = default;

  /**
   * @return The name of the interface.
   */
  [[nodiscard]] auto interface_name() const -> const std::string& { return interface_name_; }

  /**
   * @return The Major version qualifier.
   */
  [[nodiscard]] auto version_major() const -> uint32_t { return version_major_; }

  /**
   * @return The Minor version qualifier.
   */
  [[nodiscard]] auto version_minor() const -> uint32_t { return version_minor_; }

  /**
   * @return The type of this Interface (Datastream or Property).
   */
  [[nodiscard]] auto interface_type() const -> InterfaceType { return interface_type_; }

  /**
   * @return The quality of the interface.
   */
  [[nodiscard]] auto ownership() const -> AstarteOwnership { return ownership_; }

  /**
   * @return The aggregation of the mappings (Individual or Object), if present.
   */
  [[nodiscard]] auto aggregation() const -> const std::optional<InterfaceAggregation>& {
    return aggregation_;
  }

  /**
   * @return The optional description.
   */
  [[nodiscard]] auto description() const -> const std::optional<std::string>& {
    return description_;
  }

  /**
   * @return The documentation string.
   */
  [[nodiscard]] auto doc() const -> const std::optional<std::string>& { return doc_; }

  /**
   * @return The vector of mappings defined for this interface.
   */
  [[nodiscard]] auto mappings() const -> const std::vector<Mapping>& { return mappings_; }

  /**
   * @brief Retrieve the mapping associated to a given path if it exists.
   *
   * @param path the Astarte interface path.
   * @return a pointer to the mapping associated with the path, an error otherwise.
   */
  [[nodiscard]] auto get_mapping(std::string_view path) const
      -> astarte_tl::expected<const Mapping*, AstarteError>;

  /**
   * @brief Validate an Astarte individual.
   *
   * @param path the Astarte interface path.
   * @param data the value to validate.
   * @param timestamp a pointer to the timestamp poiting out when the data is sent.
   * @return an error if the falidation fails, nothing otherwise.
   */
  auto validate_individual(std::string_view path, const AstarteData& data,
                           const std::chrono::system_clock::time_point* timestamp) const
      -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Validate an Astarte object.
   *
   * @param common_path common path of the Astarte interface enpoints.
   * @param object the Astarte object data to validate.
   * @param timestamp a pointer to the timestamp pointing out when the data is sent.
   * @return an error if the falidation fails, nothing otherwise.
   */
  auto validate_object(std::string_view common_path, const AstarteDatastreamObject& object,
                       const std::chrono::system_clock::time_point* timestamp) const
      -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Get the MQTT QoS from a certain mapping endpoint.
   *
   * @param path the Astarte interface path.
   * @return the QoS value, an error otherwise.
   */
  [[nodiscard]] auto get_qos(std::string_view path) const
      -> astarte_tl::expected<uint8_t, AstarteError>;

 private:
  Interface(std::string interface_name, uint32_t version_major, uint32_t version_minor,
            InterfaceType interface_type, AstarteOwnership ownership,
            std::optional<InterfaceAggregation> aggregation, std::optional<std::string> description,
            std::optional<std::string> doc, std::vector<Mapping> mappings)
      : interface_name_(std::move(interface_name)),
        version_major_(version_major),
        version_minor_(version_minor),
        interface_type_(interface_type),
        ownership_(ownership),
        aggregation_(aggregation),
        description_(std::move(description)),
        doc_(std::move(doc)),
        mappings_(std::move(mappings)) {}

  std::string interface_name_;
  uint32_t version_major_;
  uint32_t version_minor_;
  InterfaceType interface_type_;
  AstarteOwnership ownership_;
  std::optional<InterfaceAggregation> aggregation_;
  std::optional<std::string> description_;
  std::optional<std::string> doc_;
  std::vector<Mapping> mappings_;
};

/**
 * @brief Represents a collection of Astarte interface.
 *
 * The introspection represents the set of device supported interfaces.
 * See the Astarte documentation for more details.
 * https://docs.astarte-platform.org/astarte/latest/080-mqtt-v1-protocol.html#introspection
 */
class Introspection {
 public:
  /**
   * @brief Construct an empty Introspection.
   */
  Introspection() = default;

  /**
   * @brief Try to insert and Interface into the Introspection.
   *
   * @param interface The interface to add.
   * @return an error if the operation fails
   */
  auto checked_insert(Interface interface) -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Return a view over the introspection values.
   *
   * @return a view over the introspection interfaces.
   */
  [[nodiscard]] auto values() const { return std::views::values(interfaces_); }

  /**
   * @brief get an interface reference if the interface is contained in the device introspection.
   *
   * @param interface_name the interface name.
   * @return the interface reference if found inside the introspection, an error otherwise.
   */
  [[nodiscard]] auto get(const std::string& interface_name)
      -> astarte_tl::expected<Interface*, AstarteError>;

 private:
  /**
   * @brief A map containing the interfaces in the Device Introspection synced with Astarte.
   */
  std::map<std::string, Interface> interfaces_;
};

}  // namespace AstarteDeviceSdk

// ------------------------------------------------------------------------------------------------
// FORAMATTING
// ------------------------------------------------------------------------------------------------

/**
 * @brief astarte_fmt::formatter specialization for InterfaceType.
 */

template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::InterfaceType> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the InterfaceType enum.
   * @param msg The InterfaceType to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::InterfaceType& typ, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (typ) {
      case AstarteDeviceSdk::InterfaceType::kDatastream:
        astarte_fmt::format_to(out, "datastream");
        break;

      case AstarteDeviceSdk::InterfaceType::kProperty:
        astarte_fmt::format_to(out, "property");
        break;
    }

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::InterfaceType typ)
    -> std::ostream& {
  out << astarte_fmt::format("{}", typ);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for InterfaceAggregation.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::InterfaceAggregation> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the InterfaceAggregation enum.
   * @param msg The InterfaceAggregation to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::InterfaceAggregation& aggr, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (aggr) {
      case AstarteDeviceSdk::InterfaceAggregation::kIndividual:
        astarte_fmt::format_to(out, "individual");
        break;

      case AstarteDeviceSdk::InterfaceAggregation::kObject:
        astarte_fmt::format_to(out, "object");
        break;
    }

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::InterfaceAggregation aggr)
    -> std::ostream& {
  out << astarte_fmt::format("{}", aggr);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for Reliability.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::Reliability> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the Reliability enum.
   * @param msg The Reliability to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::Reliability& rel, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (rel) {
      case AstarteDeviceSdk::Reliability::kUnreliable:
        astarte_fmt::format_to(out, "unreliable");
        break;

      case AstarteDeviceSdk::Reliability::kGuaranteed:
        astarte_fmt::format_to(out, "guaranteed");
        break;

      case AstarteDeviceSdk::Reliability::kUnique:
        astarte_fmt::format_to(out, "unique");
        break;
    }

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::Reliability rel)
    -> std::ostream& {
  out << astarte_fmt::format("{}", rel);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for Retention.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::Retention> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the Retention enum.
   * @param msg The Retention to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::Retention& ret, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (ret) {
      case AstarteDeviceSdk::Retention::kDiscard:
        astarte_fmt::format_to(out, "discard");
        break;

      case AstarteDeviceSdk::Retention::kVolatile:
        astarte_fmt::format_to(out, "volatile");
        break;

      case AstarteDeviceSdk::Retention::kStored:
        astarte_fmt::format_to(out, "stored");
        break;
    }

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::Retention ret) -> std::ostream& {
  out << astarte_fmt::format("{}", ret);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for DatabaseRetentionPolicy.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::DatabaseRetentionPolicy> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the DatabaseRetentionPolicy enum.
   * @param msg The DatabaseRetentionPolicy to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::DatabaseRetentionPolicy& ret_pol, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (ret_pol) {
      case AstarteDeviceSdk::DatabaseRetentionPolicy::kNoTtl:
        astarte_fmt::format_to(out, "no_ttl");
        break;

      case AstarteDeviceSdk::DatabaseRetentionPolicy::kUseTtl:
        astarte_fmt::format_to(out, "use_ttl");
        break;
    }

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::DatabaseRetentionPolicy ret_pol)
    -> std::ostream& {
  out << astarte_fmt::format("{}", ret_pol);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for Mapping.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::Mapping> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the Mapping enum.
   * @param msg The Mapping to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-function-size)
  auto format(const AstarteDeviceSdk::Mapping& mapping, FormatContext& ctx) const {
    auto out = ctx.out();

    astarte_fmt::format_to(out, "Mapping {{");
    astarte_fmt::format_to(out, "endpoint: {}", mapping.endpoint());
    astarte_fmt::format_to(out, ", type: {}", mapping.type());
    if (auto val = mapping.explicit_timestamp()) {
      astarte_fmt::format_to(out, ", explicit_timestamp: {}", *val);
    }
    if (auto val = mapping.reliability()) {
      astarte_fmt::format_to(out, ", reliability: {}", *val);
    }
    if (auto val = mapping.retention()) {
      astarte_fmt::format_to(out, ", retention: {}", *val);
    }
    if (auto val = mapping.expiry()) {
      astarte_fmt::format_to(out, ", expiry: {}", *val);
    }
    if (auto val = mapping.database_retention_policy()) {
      astarte_fmt::format_to(out, ", database_retention_policy: {}", *val);
    }
    if (auto val = mapping.database_retention_ttl()) {
      astarte_fmt::format_to(out, ", database_retention_ttl: {}", *val);
    }
    if (auto val = mapping.allow_unset()) {
      astarte_fmt::format_to(out, ", allow_unset: {}", *val);
    }
    if (auto val = mapping.description()) {
      astarte_fmt::format_to(out, ", description: {}", *val);
    }
    if (auto val = mapping.doc()) {
      astarte_fmt::format_to(out, ", doc: {}", *val);
    }
    astarte_fmt::format_to(out, "}}");

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::Mapping& mapping)
    -> std::ostream& {
  out << astarte_fmt::format("{}", mapping);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for Interface.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::Interface> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the Interface object.
   * @param msg The Interface to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::Interface& interface, FormatContext& ctx) const {
    auto out = ctx.out();

    astarte_fmt::format_to(out, "Interface {{\n");
    astarte_fmt::format_to(out, "  interface name: {}\n", interface.interface_name());
    astarte_fmt::format_to(out, "  major version: {}\n", interface.version_major());
    astarte_fmt::format_to(out, "  minor version: {}\n", interface.version_minor());
    astarte_fmt::format_to(out, "  interface type: {}\n", interface.interface_type());
    astarte_fmt::format_to(out, "  ownership: {}\n", interface.ownership());
    auto aggr = interface.aggregation();
    if (aggr) {
      astarte_fmt::format_to(out, "  aggregation: {}\n", *aggr);
    }
    auto desc = interface.description();
    if (desc) {
      astarte_fmt::format_to(out, "  description: {}\n", *desc);
    }
    auto doc = interface.doc();
    if (doc) {
      astarte_fmt::format_to(out, "  doc: {}\n", *doc);
    }
    astarte_fmt::format_to(out, "  mappings: ");
    AstarteDeviceSdk::utils::format_vector(out, interface.mappings());
    astarte_fmt::format_to(out, "\n");

    astarte_fmt::format_to(out, "}}\n");

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::Interface& interface)
    -> std::ostream& {
  out << astarte_fmt::format("{}", interface);
  return out;
}

#endif  // ASTARTE_MQTT_INTROSPECTION_H
