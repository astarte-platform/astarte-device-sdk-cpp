// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MAPPING_H
#define ASTARTE_MAPPING_H

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

namespace astarte::device {

using json = nlohmann::json;

/// @brief Represents the reliability of an Astarte datastream.
class Reliability {
 public:
  /// @brief Underlying values for the reliability.
  enum Value : uint8_t {
    /// @brief If the transport sends the data.
    kUnreliable,
    /// @brief When we know the data has been received at least once.
    kGuaranteed,
    /// @brief When we know the data has been received exactly once.
    kUnique,
  };

  /// @brief Construct a Reliability object with default value (unreliable).
  Reliability() : value_(kUnreliable) {}

  /**
   * @brief Construct a Reliability from a specific Value.
   * @param val The reliability value.
   */
  explicit Reliability(Value val) : value_(val) {}

  /// @brief Default equality operator for comparing two Reliability objects.
  constexpr auto operator==(const Reliability& other) const -> bool = default;
  /// @brief Equality operator for comparing with a raw Value.
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  /**
   * @brief Convert the reliability to its string representation.
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
   * @brief Attempt to create a Reliability from a string.
   * @param str The string representation to parse.
   * @return An expected containing the Reliability on success, or an AstarteError on failure.
   */
  static auto try_from_str(std::string_view str)
      -> astarte_tl::expected<Reliability, AstarteError> {
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
        AstarteInvalidReliabilityError(astarte_fmt::format("reliability not valid: {}", str)));
  }

  /**
   * @brief Get the Quality of Service level associated with the reliability.
   * @return The QoS value as an 8-bit integer.
   */
  [[nodiscard]] auto get_qos() const -> int8_t { return static_cast<int8_t>(value_); }

  /**
   * @brief Deserialize Reliability from a JSON object.
   * @param json The JSON object to deserialize from.
   * @param rel The Reliability object to fill.
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

/// @brief Represents the retention policy of an Astarte datastream.
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

  /// @brief Construct a Retention object with default value (discard).
  Retention() : value_(kDiscard) {}

  /**
   * @brief Construct a Retention from a specific Value.
   * @param val The retention value.
   */
  explicit Retention(Value val) : value_(val) {}

  /// @brief Default equality operator for comparing two Retention objects.
  constexpr auto operator==(const Retention& other) const -> bool = default;
  /// @brief Equality operator for comparing with a raw Value.
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  /**
   * @brief Convert the retention to its string representation.
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
   * @brief Attempt to create a Retention from a string.
   * @param str The string representation to parse.
   * @return An expected containing the Retention on success, or an AstarteError on failure.
   */
  static auto try_from_str(std::string_view str) -> astarte_tl::expected<Retention, AstarteError> {
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
        AstarteInvalidRetentionError(astarte_fmt::format("retention not valid: {}", str)));
  }

  /**
   * @brief Deserialize Retention from a JSON object.
   * @param json The JSON object to deserialize from.
   * @param ret The Retention object to fill.
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

/// @brief Represents the database retention policy of an Astarte mapping.
class DatabaseRetentionPolicy {
 public:
  /// @brief Underlying values for the database retention policy.
  enum Value : uint8_t {
    /// @brief The data will never expire.
    kNoTtl,
    /// @brief The data will expire after a given Time To Live.
    kUseTtl,
  };

  /// @brief Construct a DatabaseRetentionPolicy object with default value (no_ttl).
  DatabaseRetentionPolicy() : value_(kNoTtl) {}

  /**
   * @brief Construct a DatabaseRetentionPolicy from a specific Value.
   * @param val The policy value.
   */
  explicit DatabaseRetentionPolicy(Value val) : value_(val) {}

  /// @brief Default equality operator for comparing two policy objects.
  constexpr auto operator==(const DatabaseRetentionPolicy& other) const -> bool = default;
  /// @brief Equality operator for comparing with a raw Value.
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  /**
   * @brief Convert the policy to its string representation.
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
   * @brief Attempt to create a DatabaseRetentionPolicy from a string.
   * @param str The string representation to parse.
   * @return An expected containing the policy on success, or an AstarteError on failure.
   */
  static auto try_from_str(std::string_view str)
      -> astarte_tl::expected<DatabaseRetentionPolicy, AstarteError> {
    if (str == "no_ttl") {
      return DatabaseRetentionPolicy(kNoTtl);
    }
    if (str == "use_ttl") {
      return DatabaseRetentionPolicy(kUseTtl);
    }
    return astarte_tl::unexpected(AstarteInvalidDatabaseRetentionPolicyError(
        astarte_fmt::format("database retention policy not valid: {}", str)));
  }

  /**
   * @brief Deserialize DatabaseRetentionPolicy from a JSON object.
   * @param json The JSON object to deserialize from.
   * @param pol The DatabaseRetentionPolicy object to fill.
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

  /**
   * @brief Construct a Mapping from a json.
   * @param json The json structure to parse
   * @return A Mapping object representation of the json data, an error otherwise.
   */
  static auto try_from_json(const json& json) -> astarte_tl::expected<Mapping, AstarteError>;

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

}  // namespace astarte::device

// ------------------------------------------------------------------------------------------------
// FORAMATTING
// ------------------------------------------------------------------------------------------------

/// @brief astarte_fmt::formatter specialization for Reliability.
template <>
struct astarte_fmt::formatter<astarte::device::Reliability> {
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
  auto format(const astarte::device::Reliability& rel, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", rel.to_string());
  }
};

inline auto operator<<(std::ostream& out, const astarte::device::Reliability rel) -> std::ostream& {
  out << astarte_fmt::format("{}", rel);
  return out;
}

/// @brief astarte_fmt::formatter specialization for Retention.
template <>
struct astarte_fmt::formatter<astarte::device::Retention> {
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
  auto format(const astarte::device::Retention& ret, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", ret.to_string());
  }
};

inline auto operator<<(std::ostream& out, const astarte::device::Retention ret) -> std::ostream& {
  out << astarte_fmt::format("{}", ret);
  return out;
}

/// @brief astarte_fmt::formatter specialization for DatabaseRetentionPolicy.
template <>
struct astarte_fmt::formatter<astarte::device::DatabaseRetentionPolicy> {
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
  auto format(const astarte::device::DatabaseRetentionPolicy& ret_pol, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", ret_pol.to_string());
  }
};

inline auto operator<<(std::ostream& out, const astarte::device::DatabaseRetentionPolicy ret_pol)
    -> std::ostream& {
  out << astarte_fmt::format("{}", ret_pol);
  return out;
}

/// @brief astarte_fmt::formatter specialization for Mapping.
template <>
struct astarte_fmt::formatter<astarte::device::Mapping> {
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
  auto format(const astarte::device::Mapping& mapping, FormatContext& ctx) const {
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

inline auto operator<<(std::ostream& out, const astarte::device::Mapping& mapping)
    -> std::ostream& {
  out << astarte_fmt::format("{}", mapping);
  return out;
}

#endif  // ASTARTE_MAPPING_H
