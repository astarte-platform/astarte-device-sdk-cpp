// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_FORMATTER_H
#define ASTARTE_MQTT_FORMATTER_H

#include <chrono>
#include <cstddef>
#include <type_traits>

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) ||    \
     (!defined(_MSVC_LANG) && __cplusplus >= 202002L)) && \
    (__has_include(<format>))
#include <format>
#define ASTARTE_NS_FORMAT std
#else                        // (__cplusplus >= 202002L) && (__has_include(<format>))
#include <spdlog/fmt/fmt.h>  // NOLINT: avoid clang-tidy warning regarding fmt library not used directly

#include <iomanip>
#include <sstream>
#define ASTARTE_NS_FORMAT fmt
#endif  // (__cplusplus >= 202002L) && (__has_include(<format>))

#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/mqtt/introspection.hpp"
#include "astarte_device_sdk/ownership.hpp"

/// @cond Doxygen should skip checking ASTARTE_NS_FORMAT::formatter due to internal inconsistency
/// parsing

/**
 * @brief ASTARTE_NS_FORMAT::formatter specialization for InterfaceType.
 */
template <>
struct ASTARTE_NS_FORMAT::formatter<AstarteDeviceSdk::InterfaceType> {
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
        ASTARTE_NS_FORMAT::format_to(out, "datastream");
        break;

      case AstarteDeviceSdk::InterfaceType::kProperty:
        ASTARTE_NS_FORMAT::format_to(out, "property");
        break;
    }

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::InterfaceType typ) {
  out << ASTARTE_NS_FORMAT::format("{}", typ);
  return out;
}

/**
 * @brief ASTARTE_NS_FORMAT::formatter specialization for Aggregation.
 */
template <>
struct ASTARTE_NS_FORMAT::formatter<AstarteDeviceSdk::Aggregation> {
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
        ASTARTE_NS_FORMAT::format_to(out, "individual");
        break;

      case AstarteDeviceSdk::Aggregation::kObject:
        ASTARTE_NS_FORMAT::format_to(out, "object");
        break;
    }

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::Aggregation aggr) {
  out << ASTARTE_NS_FORMAT::format("{}", aggr);
  return out;
}

/**
 * @brief ASTARTE_NS_FORMAT::formatter specialization for Reliability.
 */
template <>
struct ASTARTE_NS_FORMAT::formatter<AstarteDeviceSdk::Reliability> {
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
        ASTARTE_NS_FORMAT::format_to(out, "unreliable");
        break;

      case AstarteDeviceSdk::Reliability::kGuaranteed:
        ASTARTE_NS_FORMAT::format_to(out, "guaranteed");
        break;

      case AstarteDeviceSdk::Reliability::kUnique:
        ASTARTE_NS_FORMAT::format_to(out, "unique");
        break;
    }

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::Reliability rel) {
  out << ASTARTE_NS_FORMAT::format("{}", rel);
  return out;
}

/**
 * @brief ASTARTE_NS_FORMAT::formatter specialization for Retention.
 */
template <>
struct ASTARTE_NS_FORMAT::formatter<AstarteDeviceSdk::Retention> {
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
        ASTARTE_NS_FORMAT::format_to(out, "discard");
        break;

      case AstarteDeviceSdk::Retention::kVolatile:
        ASTARTE_NS_FORMAT::format_to(out, "volatile");
        break;

      case AstarteDeviceSdk::Retention::kStored:
        ASTARTE_NS_FORMAT::format_to(out, "stored");
        break;
    }

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::Retention ret) {
  out << ASTARTE_NS_FORMAT::format("{}", ret);
  return out;
}

/**
 * @brief ASTARTE_NS_FORMAT::formatter specialization for DatabaseRetentionPolicy.
 */
template <>
struct ASTARTE_NS_FORMAT::formatter<AstarteDeviceSdk::DatabaseRetentionPolicy> {
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
        ASTARTE_NS_FORMAT::format_to(out, "no_ttl");
        break;

      case AstarteDeviceSdk::DatabaseRetentionPolicy::kUseTtl:
        ASTARTE_NS_FORMAT::format_to(out, "use_ttl");
        break;
    }

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out,
                                const AstarteDeviceSdk::DatabaseRetentionPolicy ret_pol) {
  out << ASTARTE_NS_FORMAT::format("{}", ret_pol);
  return out;
}

/**
 * @brief ASTARTE_NS_FORMAT::formatter specialization for Mapping.
 */
template <>
struct ASTARTE_NS_FORMAT::formatter<AstarteDeviceSdk::Mapping> {
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

    ASTARTE_NS_FORMAT::format_to(out, "Mapping {{");
    ASTARTE_NS_FORMAT::format_to(out, "endpoint: {}", mapping.endpoint);
    ASTARTE_NS_FORMAT::format_to(out, ", type: {}", mapping.mapping_type);
    if (mapping.explicit_timestamp) {
      ASTARTE_NS_FORMAT::format_to(out, ", explicit_timestamp: {}",
                                   mapping.explicit_timestamp.value());
    }
    if (mapping.reliability) {
      ASTARTE_NS_FORMAT::format_to(out, ", reliability: {}", mapping.reliability.value());
    }
    if (mapping.retention) {
      ASTARTE_NS_FORMAT::format_to(out, ", retention: {}", mapping.retention.value());
    }
    if (mapping.expiry) {
      ASTARTE_NS_FORMAT::format_to(out, ", expiry: {}", mapping.expiry.value());
    }
    if (mapping.database_retention_policy) {
      ASTARTE_NS_FORMAT::format_to(out, ", database_retention_policy: {}",
                                   mapping.database_retention_policy.value());
    }
    if (mapping.database_retention_ttl) {
      ASTARTE_NS_FORMAT::format_to(out, ", database_retention_ttl: {}",
                                   mapping.database_retention_ttl.value());
    }
    if (mapping.allow_unset) {
      ASTARTE_NS_FORMAT::format_to(out, ", allow_unset: {}", mapping.allow_unset.value());
    }
    if (mapping.description) {
      ASTARTE_NS_FORMAT::format_to(out, ", description: {}", mapping.description.value());
    }
    if (mapping.doc) {
      ASTARTE_NS_FORMAT::format_to(out, ", doc: {}", mapping.doc.value());
    }
    ASTARTE_NS_FORMAT::format_to(out, "}}");

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::Mapping mapping) {
  out << ASTARTE_NS_FORMAT::format("{}", mapping);
  return out;
}

/**
 * @brief ASTARTE_NS_FORMAT::formatter specialization for Interface.
 */
template <>
struct ASTARTE_NS_FORMAT::formatter<AstarteDeviceSdk::Interface> {
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

    ASTARTE_NS_FORMAT::format_to(out, "Interface {{\n");
    ASTARTE_NS_FORMAT::format_to(out, "  interface name: {}\n", interface.interface_name);
    ASTARTE_NS_FORMAT::format_to(out, "  major version: {}\n", interface.version_major);
    ASTARTE_NS_FORMAT::format_to(out, "  minor version: {}\n", interface.version_minor);
    ASTARTE_NS_FORMAT::format_to(out, "  interface type: {}\n", interface.interface_type);
    ASTARTE_NS_FORMAT::format_to(out, "  ownership: {}\n", interface.ownership);
    if (interface.aggregation) {
      ASTARTE_NS_FORMAT::format_to(out, "  aggregation: {}\n", interface.aggregation.value());
    }
    if (interface.description) {
      ASTARTE_NS_FORMAT::format_to(out, "  description: {}\n", interface.description.value());
    }
    if (interface.doc) {
      ASTARTE_NS_FORMAT::format_to(out, "  doc: {}\n", interface.doc.value());
    }
    ASTARTE_NS_FORMAT::format_to(out, "  mappings: ");
    utils::format_vector(out, interface.mappings);
    ASTARTE_NS_FORMAT::format_to(out, "\n");

    ASTARTE_NS_FORMAT::format_to(out, "}}\n");

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out, const AstarteDeviceSdk::Interface interface) {
  out << ASTARTE_NS_FORMAT::format("{}", interface);
  return out;
}

/// @endcond

#endif  // ASTARTE_MQTT_FORMATTER_H
