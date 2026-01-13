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

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::InterfaceType typ) {
  out << astarte_fmt::format("{}", typ);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for Aggregation.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::Aggregation> {
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
   * @brief Format the Aggregation enum.
   * @param msg The Aggregation to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::Aggregation& aggr, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (aggr) {
      case AstarteDeviceSdk::Aggregation::kIndividual:
        astarte_fmt::format_to(out, "individual");
        break;

      case AstarteDeviceSdk::Aggregation::kObject:
        astarte_fmt::format_to(out, "object");
        break;
    }

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::Aggregation aggr) {
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

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::Reliability rel) {
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

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::Retention ret) {
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

inline std::ostream& operator<<(std::ostream& out,
                                const AstarteDeviceSdk::DatabaseRetentionPolicy ret_pol) {
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
  auto format(const AstarteDeviceSdk::Mapping& mapping, FormatContext& ctx) const {
    auto out = ctx.out();

    astarte_fmt::format_to(out, "Mapping {{");
    astarte_fmt::format_to(out, "endpoint: {}", mapping.endpoint_);
    astarte_fmt::format_to(out, ", type: {}", mapping.type_);
    if (mapping.explicit_timestamp_) {
      astarte_fmt::format_to(out, ", explicit_timestamp: {}", mapping.explicit_timestamp_.value());
    }
    if (mapping.reliability_) {
      astarte_fmt::format_to(out, ", reliability: {}", mapping.reliability_.value());
    }
    if (mapping.retention_) {
      astarte_fmt::format_to(out, ", retention: {}", mapping.retention_.value());
    }
    if (mapping.expiry_) {
      astarte_fmt::format_to(out, ", expiry: {}", mapping.expiry_.value());
    }
    if (mapping.database_retention_policy_) {
      astarte_fmt::format_to(out, ", database_retention_policy: {}",
                             mapping.database_retention_policy_.value());
    }
    if (mapping.database_retention_ttl_) {
      astarte_fmt::format_to(out, ", database_retention_ttl: {}",
                             mapping.database_retention_ttl_.value());
    }
    if (mapping.allow_unset_) {
      astarte_fmt::format_to(out, ", allow_unset: {}", mapping.allow_unset_.value());
    }
    if (mapping.description_) {
      astarte_fmt::format_to(out, ", description: {}", mapping.description_.value());
    }
    if (mapping.doc_) {
      astarte_fmt::format_to(out, ", doc: {}", mapping.doc_.value());
    }
    astarte_fmt::format_to(out, "}}");

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::Mapping mapping) {
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
    if (interface.aggregation()) {
      astarte_fmt::format_to(out, "  aggregation: {}\n", interface.aggregation().value());
    }
    if (interface.description()) {
      astarte_fmt::format_to(out, "  description: {}\n", interface.description().value());
    }
    if (interface.doc()) {
      astarte_fmt::format_to(out, "  doc: {}\n", interface.doc().value());
    }
    astarte_fmt::format_to(out, "  mappings: ");
    utils::format_vector(out, interface.mappings());
    astarte_fmt::format_to(out, "\n");

    astarte_fmt::format_to(out, "}}\n");

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::Interface interface) {
  out << astarte_fmt::format("{}", interface);
  return out;
}

/// @endcond

#endif  // ASTARTE_MQTT_FORMATTER_H
