// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MAPPING_H
#define ASTARTE_MAPPING_H

/**
 * @file private/mqtt/mapping.hpp
 * @brief Definition of Astarte Mappings and related types.
 *
 * @details This file defines the `Mapping` class, which represents a single mapping
 * within an Astarte Interface, along with auxiliary classes for reliability, retention,
 * and database policies.
 */

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <format>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <ostream>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/type.hpp"

namespace astarte::device::mqtt {

using json = nlohmann::json;

/**
 * @brief Represents the reliability of an Astarte datastream.
 * @details Determines the guarantee level of message delivery (e.g., QoS 0, 1, or 2).
 */
class Reliability {
 public:
  /// @brief Underlying values for the reliability.
  enum Value : uint8_t {
    /// @brief If the transport sends the data (QoS 0).
    kUnreliable,
    /// @brief When we know the data has been received at least once (QoS 1).
    kGuaranteed,
    /// @brief When we know the data has been received exactly once (QoS 2).
    kUnique,
  };

  /// @brief Constructs a Reliability object with default value (unreliable).
  Reliability() : value_(kUnreliable) {}

  /**
   * @brief Constructs a Reliability from a specific Value.
   * @param[in] val The reliability value.
   */
  explicit Reliability(Value val) : value_(val) {}

  /// @brief Default equality operator for comparing two Reliability objects.
  constexpr auto operator==(const Reliability& other) const -> bool = default;
  /// @brief Equality operator for comparing with a raw Value.
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  /**
   * @brief Converts the reliability to its string representation.
   * @return A string_view containing "unreliable", "guaranteed", or "unique".
   */
  [[nodiscard]] constexpr auto to_string() const -> std::string_view {
    switch (value_) {
      case kUnreliable:
        return "unreliable";
      case kGuaranteed:
        return "guaranteed";
      case kUnique:
        return "unique";
    }
    return "unreachable";
  }

  /**
   * @brief Attempts to create a Reliability from a string.
   * @param[in] str The string representation to parse.
   * @return An expected containing the Reliability on success, or an Error on failure.
   */
  static auto try_from_str(std::string_view str) -> astarte_tl::expected<Reliability, Error> {
    if (str == "unreliable") {
      return Reliability(kUnreliable);
    }
    if (str == "guaranteed") {
      return Reliability(kGuaranteed);
    }
    if (str == "unique") {
      return Reliability(kUnique);
    }
    return astarte_tl::unexpected(
        InvalidReliabilityError(astarte_fmt::format("reliability not valid: {}", str)));
  }

  /**
   * @brief Gets the Quality of Service (QoS) level associated with the reliability.
   * @return The QoS value as an 8-bit integer (0, 1, or 2).
   */
  [[nodiscard]] auto get_qos() const -> int8_t { return static_cast<int8_t>(value_); }

  /**
   * @brief Deserializes Reliability from a JSON object.
   * @param[in] json The JSON object to deserialize from.
   * @param[out] rel The Reliability object to fill.
   */
  friend void from_json(const nlohmann::json& json, Reliability& rel) {
    if (json.is_string()) {
      auto res = try_from_str(json.get<std::string>());
      if (res) {
        rel = res.value();
        return;
      }
    }
    rel.value_ = kUnreliable;
  }

 private:
  Value value_;
};

/**
 * @brief Represents the retention policy of an Astarte datastream.
 * @details Determines how messages are handled when the transport is temporarily unavailable.
 */
class Retention {
 public:
  /// @brief Underlying values for the retention.
  enum Value : uint8_t {
    /// @brief Data is discarded if the transport is incapable of delivery.
    kDiscard,
    /// @brief Data is kept in a cache in memory.
    kVolatile,
    /// @brief Data is kept on non-volatile storage.
    kStored,
  };

  /// @brief Constructs a Retention object with default value (discard).
  Retention() : value_(kDiscard) {}

  /**
   * @brief Constructs a Retention from a specific Value.
   * @param[in] val The retention value.
   */
  explicit Retention(Value val) : value_(val) {}

  /// @brief Default equality operator for comparing two Retention objects.
  constexpr auto operator==(const Retention& other) const -> bool = default;
  /// @brief Equality operator for comparing with a raw Value.
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  /**
   * @brief Converts the retention to its string representation.
   * @return A string_view containing "discard", "volatile", or "stored".
   */
  [[nodiscard]] constexpr auto to_string() const -> std::string_view {
    switch (value_) {
      case kDiscard:
        return "discard";
      case kVolatile:
        return "volatile";
      case kStored:
        return "stored";
    }
    return "unreachable";
  }

  /**
   * @brief Attempts to create a Retention from a string.
   * @param[in] str The string representation to parse.
   * @return An expected containing the Retention on success, or an Error on failure.
   */
  static auto try_from_str(std::string_view str) -> astarte_tl::expected<Retention, Error> {
    if (str == "discard") {
      return Retention(kDiscard);
    }
    if (str == "volatile") {
      return Retention(kVolatile);
    }
    if (str == "stored") {
      return Retention(kStored);
    }
    return astarte_tl::unexpected(
        InvalidRetentionError(astarte_fmt::format("retention not valid: {}", str)));
  }

  /**
   * @brief Deserializes Retention from a JSON object.
   * @param[in] json The JSON object to deserialize from.
   * @param[out] ret The Retention object to fill.
   */
  friend void from_json(const nlohmann::json& json, Retention& ret) {
    if (json.is_string()) {
      auto res = try_from_str(json.get<std::string>());
      if (res) {
        ret = res.value();
        return;
      }
    }
    ret.value_ = kDiscard;
  }

 private:
  Value value_;
};

/**
 * @brief Represents the database retention policy of an Astarte mapping.
 * @details Determines if data stored in the Astarte database should expire.
 */
class DatabaseRetentionPolicy {
 public:
  /// @brief Underlying values for the database retention policy.
  enum Value : uint8_t {
    /// @brief The data will never expire.
    kNoTtl,
    /// @brief The data will expire after a given Time To Live.
    kUseTtl,
  };

  /// @brief Constructs a DatabaseRetentionPolicy object with default value (no_ttl).
  DatabaseRetentionPolicy() : value_(kNoTtl) {}

  /**
   * @brief Constructs a DatabaseRetentionPolicy from a specific Value.
   * @param[in] val The policy value.
   */
  explicit DatabaseRetentionPolicy(Value val) : value_(val) {}

  /// @brief Default equality operator for comparing two policy objects.
  constexpr auto operator==(const DatabaseRetentionPolicy& other) const -> bool = default;
  /// @brief Equality operator for comparing with a raw Value.
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  /**
   * @brief Converts the policy to its string representation.
   * @return A string_view containing "no_ttl" or "use_ttl".
   */
  [[nodiscard]] constexpr auto to_string() const -> std::string_view {
    switch (value_) {
      case kNoTtl:
        return "no_ttl";
      case kUseTtl:
        return "use_ttl";
    }
    return "unreachable";
  }

  /**
   * @brief Attempts to create a DatabaseRetentionPolicy from a string.
   * @param[in] str The string representation to parse.
   * @return An expected containing the policy on success, or an Error on failure.
   */
  static auto try_from_str(std::string_view str)
      -> astarte_tl::expected<DatabaseRetentionPolicy, Error> {
    if (str == "no_ttl") {
      return DatabaseRetentionPolicy(kNoTtl);
    }
    if (str == "use_ttl") {
      return DatabaseRetentionPolicy(kUseTtl);
    }
    return astarte_tl::unexpected(InvalidDatabaseRetentionPolicyError(
        astarte_fmt::format("database retention policy not valid: {}", str)));
  }

  /**
   * @brief Deserializes DatabaseRetentionPolicy from a JSON object.
   * @param[in] json The JSON object to deserialize from.
   * @param[out] pol The DatabaseRetentionPolicy object to fill.
   */
  friend void from_json(const nlohmann::json& json, DatabaseRetentionPolicy& pol) {
    if (json.is_string()) {
      auto res = try_from_str(json.get<std::string>());
      if (res) {
        pol = res.value();
        return;
      }
    }
    pol.value_ = kNoTtl;
  }

 private:
  Value value_;
};

/**
 * @brief Represents a single Astarte mapping within an interface.
 *
 * @details A mapping defines a specific data endpoint (e.g., `/sensors/temp`), its type,
 * reliability, and other configuration parameters that dictate how data is exchanged.
 */
class Mapping {
 public:
  /**
   * @brief Constructs a Mapping object.
   *
   * @param[in] endpoint The endpoint pattern (can be parametrized).
   * @param[in] type The data type accepted by this mapping.
   * @param[in] explicit_timestamp Whether the client should provide an explicit timestamp.
   * @param[in] reliability The QoS level for data transmission.
   * @param[in] retention The policy for retaining unsent messages.
   * @param[in] expiry The expiration time for retained messages.
   * @param[in] database_retention_policy The policy for database storage retention.
   * @param[in] database_retention_ttl The TTL for database storage.
   * @param[in] allow_unset Whether this property can be unset.
   * @param[in] description A description of the mapping.
   * @param[in] doc Documentation for the mapping.
   */
  Mapping(std::string endpoint, Type type, std::optional<bool> explicit_timestamp,
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
   * @brief Checks that the mapping endpoint matches a given path.
   *
   * @param[in] path The Astarte interface path to check.
   * @return True if the mapping endpoint matches the path, false otherwise.
   */
  [[nodiscard]] auto match_path(std::string_view path) const -> bool;

  /**
   * @brief Checks that the Astarte data matches the mapping type.
   *
   * @param[in] data The Data object to check.
   * @return An expected void on success, or an Error if the types mismatch.
   */
  [[nodiscard]] auto check_data_type(const Data& data) const -> astarte_tl::expected<void, Error>;

  /**
   * @brief Gets the path of the mapping.
   * @details It can be parametrized (e.g. `/foo/%{path}/baz`).
   * @return A constant reference to the endpoint string.
   */
  [[nodiscard]] auto endpoint() const -> const std::string& { return endpoint_; }

  /**
   * @brief Gets the type of the mapping.
   * @details This represents the data type that will be published on the mapping.
   * @return The Astarte Type.
   */
  [[nodiscard]] auto type() const -> Type { return type_; }

  /**
   * @brief Checks if an explicit timestamp is allowed.
   * @return An optional boolean indicating the configuration.
   */
  [[nodiscard]] auto explicit_timestamp() const -> std::optional<bool> {
    return explicit_timestamp_;
  }

  /**
   * @brief Gets the reliability configuration.
   * @details Useful only with datastreams. Defines delivery guarantees (unreliable, guaranteed,
   * unique).
   * @return An optional Reliability object.
   */
  [[nodiscard]] auto reliability() const -> std::optional<Reliability> { return reliability_; }

  /**
   * @brief Gets the retention configuration.
   * @details Useful only with datastreams. Defines behavior when data cannot be immediately
   * delivered.
   * @return An optional Retention object.
   */
  [[nodiscard]] auto retention() const -> std::optional<Retention> { return retention_; }

  /**
   * @brief Gets the expiry for retained data.
   * @details Useful when retention is stored. Defines validity duration in seconds.
   * @return An optional expiry duration.
   */
  [[nodiscard]] auto expiry() const -> std::optional<int64_t> { return expiry_; }

  /**
   * @brief Gets the database retention policy.
   * @details Useful only with datastreams. Defines if data expires from the Astarte database.
   * @return An optional DatabaseRetentionPolicy object.
   */
  [[nodiscard]] auto database_retention_policy() const -> std::optional<DatabaseRetentionPolicy> {
    return database_retention_policy_;
  }

  /**
   * @brief Gets the database retention TTL.
   * @details Defines how many seconds data is kept in the database if policy is `use_ttl`.
   * @return An optional TTL in seconds.
   */
  [[nodiscard]] auto database_retention_ttl() const -> std::optional<int64_t> {
    return database_retention_ttl_;
  }

  /**
   * @brief Checks if the property allows unsetting.
   * @details Used only with properties.
   * @return An optional boolean.
   */
  [[nodiscard]] auto allow_unset() const -> std::optional<bool> { return allow_unset_; }

  /**
   * @brief Gets the description of the mapping.
   * @return An optional string description.
   */
  [[nodiscard]] auto description() const -> const std::optional<std::string>& {
    return description_;
  }

  /**
   * @brief Gets the documentation string.
   * @return An optional string containing documentation.
   */
  [[nodiscard]] auto doc() const -> const std::optional<std::string>& { return doc_; }

  /**
   * @brief Constructs a Mapping from a JSON object.
   * @param[in] json The JSON structure to parse.
   * @return An expected Mapping object on success, or an Error on failure.
   */
  static auto try_from_json(const json& json) -> astarte_tl::expected<Mapping, Error>;

 private:
  std::string endpoint_;
  Type type_;
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

}  // namespace astarte::device::mqtt

// ------------------------------------------------------------------------------------------------
// FORMATTING
// ------------------------------------------------------------------------------------------------

/// @brief astarte_fmt::formatter specialization for Reliability.
template <>
struct astarte_fmt::formatter<astarte::device::mqtt::Reliability> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the Reliability enum.
   * @param[in] rel The Reliability to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::mqtt::Reliability& rel, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", rel.to_string());
  }
};

inline auto operator<<(std::ostream& out, const astarte::device::mqtt::Reliability rel)
    -> std::ostream& {
  out << astarte_fmt::format("{}", rel);
  return out;
}

/// @brief astarte_fmt::formatter specialization for Retention.
template <>
struct astarte_fmt::formatter<astarte::device::mqtt::Retention> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the Retention enum.
   * @param[in] ret The Retention to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::mqtt::Retention& ret, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", ret.to_string());
  }
};

inline auto operator<<(std::ostream& out, const astarte::device::mqtt::Retention ret)
    -> std::ostream& {
  out << astarte_fmt::format("{}", ret);
  return out;
}

/// @brief astarte_fmt::formatter specialization for DatabaseRetentionPolicy.
template <>
struct astarte_fmt::formatter<astarte::device::mqtt::DatabaseRetentionPolicy> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the DatabaseRetentionPolicy enum.
   * @param[in] ret_pol The DatabaseRetentionPolicy to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::mqtt::DatabaseRetentionPolicy& ret_pol,
              FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", ret_pol.to_string());
  }
};

inline auto operator<<(std::ostream& out,
                       const astarte::device::mqtt::DatabaseRetentionPolicy ret_pol)
    -> std::ostream& {
  out << astarte_fmt::format("{}", ret_pol);
  return out;
}

/// @brief astarte_fmt::formatter specialization for Mapping.
template <>
struct astarte_fmt::formatter<astarte::device::mqtt::Mapping> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the Mapping object.
   * @param[in] mapping The Mapping to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-function-size)
  auto format(const astarte::device::mqtt::Mapping& mapping, FormatContext& ctx) const {
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

inline auto operator<<(std::ostream& out, const astarte::device::mqtt::Mapping& mapping)
    -> std::ostream& {
  out << astarte_fmt::format("{}", mapping);
  return out;
}

#endif  // ASTARTE_MAPPING_H
