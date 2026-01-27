// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_FORMATTER_H
#define ASTARTE_MQTT_FORMATTER_H

#include <chrono>
#include <cstddef>
#include <map>
#include <type_traits>

#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "mqtt/introspection.hpp"

/// @cond Doxygen should skip checking astarte_fmt::formatter due to internal inconsistency
/// parsing

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
    utils::format_vector(out, interface.mappings());
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

/// @endcond

#endif  // ASTARTE_MQTT_FORMATTER_H
