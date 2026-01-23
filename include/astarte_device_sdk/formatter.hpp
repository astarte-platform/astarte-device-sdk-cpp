// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_FORMATTER_H
#define ASTARTE_FORMATTER_H

#include <chrono>
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <variant>

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) ||    \
     (!defined(_MSVC_LANG) && __cplusplus >= 202002L)) && \
    (__has_include(<format>))
#include <format>
namespace astarte_fmt = ::std;
#else  // (__cplusplus >= 202002L) && (__has_include(<format>))
// NOLINTNEXTLINE: avoid clang-tidy warning regarding fmt library not used directly
#include <spdlog/fmt/fmt.h>

#include <iomanip>
#include <sstream>
namespace astarte_fmt = ::fmt;
#endif  // (__cplusplus >= 202002L) && (__has_include(<format>))

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "astarte_device_sdk/type.hpp"

/**
 * @brief Contain utility functions for formatting data.
 */
namespace utils {
// These functions are only used for pretty printing
// NOLINTBEGIN(concurrency-mt-unsafe)
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
/**
 * @brief Format a vector of bytes into a Base64 string literal.
 * @tparam OutputIt The type of the output iterator.
 * @param out Reference to the output iterator where the result is written.
 * @param data The vector of bytes to format.
 */
template <typename OutputIt>
void format_base64(OutputIt& out, const std::vector<uint8_t>& data) {
  static constexpr std::string_view base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/";

  size_t idx = 0;
  const size_t len = data.size();

  out = astarte_fmt::format_to(out, "\"");

  while (idx + 2 < len) {
    const uint32_t chunk = (data[idx] << 16) | (data[idx + 1] << 8) | data[idx + 2];
    out = astarte_fmt::format_to(out, "{}{}{}{}", base64_chars[(chunk >> 18) & 0x3F],
                                 base64_chars[(chunk >> 12) & 0x3F],
                                 base64_chars[(chunk >> 6) & 0x3F], base64_chars[chunk & 0x3F]);
    idx += 3;
  }

  if (idx < len) {
    uint32_t chunk = data[idx] << 16;
    if (idx + 1 < len) {
      chunk |= data[idx + 1] << 8;
    }

    out = astarte_fmt::format_to(out, "{}{}", base64_chars[(chunk >> 18) & 0x3F],
                                 base64_chars[(chunk >> 12) & 0x3F]);
    if (idx + 1 < len) {
      out = astarte_fmt::format_to(out, "{}=", base64_chars[(chunk >> 6) & 0x3F]);
    } else {
      out = astarte_fmt::format_to(out, "==");
    }
  }

  out = astarte_fmt::format_to(out, "\"");
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

/**
 * @brief Format a timestamp into an ISO 8601 string literal.
 * @tparam OutputIt The type of the output iterator.
 * @param out Reference to the output iterator where the result is written.
 * @param data The time_point to format.
 */
template <typename OutputIt>
void format_timestamp(OutputIt& out, const std::chrono::system_clock::time_point& data) {
  out = astarte_fmt::format_to(out, "\"");
#if (__cplusplus >= 202002L) && (__has_include(<format>))
  out = astarte_fmt::format_to(
      out, "{}",
      astarte_fmt::format("{0:%F}T{0:%T}Z",
                          std::chrono::time_point_cast<std::chrono::milliseconds>(data)));
#else   // (__cplusplus >= 202002L) && (__has_include(<format>))
  const std::time_t time = std::chrono::system_clock::to_time_t(data);
  const std::tm utc_tm = *std::gmtime(&time);
  std::stringstream stream;
  stream << std::put_time(&utc_tm, "%FT%T.000Z");
  out = astarte_fmt::format_to(out, "{}", stream.str());
#endif  // (__cplusplus >= 202002L) && (__has_include(<format>))
  out = astarte_fmt::format_to(out, "\"");
}

/**
 * @brief Format a generic data type into an output iterator.
 * @tparam OutputIt The type of the output iterator.
 * @tparam T The type of the element.
 * @param out Reference to the output iterator where the result is written.
 * @param data The element to format.
 */
template <typename OutputIt, typename T>
void format_data(OutputIt& out, const T& data) {
  if constexpr (std::is_same_v<T, bool>) {
    astarte_fmt::format_to(out, "{}", (data ? "true" : "false"));
  } else if constexpr (std::is_same_v<T, std::string>) {
    astarte_fmt::format_to(out, R"("{}")", data);
  } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
    format_base64(out, data);
  } else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {
    format_timestamp(out, data);
  } else {  // default format case
    astarte_fmt::format_to(out, "{}", data);
  }
}

/**
 * @brief Format a generic vector into a comma-separated list in brackets.
 * @tparam OutputIt The type of the output iterator.
 * @tparam T The type of elements in the vector.
 * @param out Reference to the output iterator where the result is written.
 * @param data The vector to format.
 */
template <typename OutputIt, typename T>
void format_vector(OutputIt& out, const std::vector<T>& data) {
  out = astarte_fmt::format_to(out, "[");
  for (size_t i = 0; i < data.size(); ++i) {
    format_data(out, data[i]);
    if (i != data.size() - 1) {
      out = astarte_fmt::format_to(out, ", ");
    }
  }
  out = astarte_fmt::format_to(out, "]");
}
// NOLINTEND(concurrency-mt-unsafe)

}  // namespace utils

/// @cond Doxygen should skip checking astarte_fmt::formatter due to internal inconsistency
/// parsing

/**
 * @brief astarte_fmt::formatter specialization for AstarteDeviceSdk::AstarteData.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::AstarteData> {
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
   * @brief Format the AstarteData variant-like object by dispatching to the correct formatter.
   * @param data The AstarteData to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-function-size)
  auto format(const AstarteDeviceSdk::AstarteData& data, FormatContext& ctx) const {
    auto out = ctx.out();

    if (std::holds_alternative<int32_t>(data.get_raw_data())) {
      out = astarte_fmt::format_to(out, "{}", std::get<int32_t>(data.get_raw_data()));
    } else if (std::holds_alternative<int64_t>(data.get_raw_data())) {
      out = astarte_fmt::format_to(out, "{}", std::get<int64_t>(data.get_raw_data()));
    } else if (std::holds_alternative<double>(data.get_raw_data())) {
      out = astarte_fmt::format_to(out, "{}", std::get<double>(data.get_raw_data()));
    } else if (std::holds_alternative<bool>(data.get_raw_data())) {
      const auto* bool_str = (std::get<bool>(data.get_raw_data()) ? "true" : "false");
      out = astarte_fmt::format_to(out, "{}", bool_str);
    } else if (std::holds_alternative<std::string>(data.get_raw_data())) {
      out = astarte_fmt::format_to(out, R"("{}")", std::get<std::string>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<uint8_t>>(data.get_raw_data())) {
      utils::format_base64(out, std::get<std::vector<uint8_t>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::chrono::system_clock::time_point>(data.get_raw_data())) {
      utils::format_timestamp(out,
                              std::get<std::chrono::system_clock::time_point>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<int32_t>>(data.get_raw_data())) {
      utils::format_vector(out, std::get<std::vector<int32_t>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<int64_t>>(data.get_raw_data())) {
      utils::format_vector(out, std::get<std::vector<int64_t>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<double>>(data.get_raw_data())) {
      utils::format_vector(out, std::get<std::vector<double>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<bool>>(data.get_raw_data())) {
      utils::format_vector(out, std::get<std::vector<bool>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<std::string>>(data.get_raw_data())) {
      utils::format_vector(out, std::get<std::vector<std::string>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<std::vector<uint8_t>>>(data.get_raw_data())) {
      utils::format_vector(out, std::get<std::vector<std::vector<uint8_t>>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<std::chrono::system_clock::time_point>>(
                   data.get_raw_data())) {
      utils::format_vector(
          out, std::get<std::vector<std::chrono::system_clock::time_point>>(data.get_raw_data()));
    }

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::AstarteData& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for AstarteDeviceSdk::AstarteType.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::AstarteType> {
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
   * @brief Format the AstarteType enum to its string representation.
   * @param typ The AstarteType to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteType& typ, FormatContext& ctx) const {
    std::string_view name = "Unknown Type";

    switch (typ) {
      case AstarteDeviceSdk::AstarteType::kBinaryBlob:
        name = "BinaryBlob";
        break;
      case AstarteDeviceSdk::AstarteType::kBoolean:
        name = "Boolean";
        break;
      case AstarteDeviceSdk::AstarteType::kDatetime:
        name = "Datetime";
        break;
      case AstarteDeviceSdk::AstarteType::kDouble:
        name = "Double";
        break;
      case AstarteDeviceSdk::AstarteType::kInteger:
        name = "Integer";
        break;
      case AstarteDeviceSdk::AstarteType::kLongInteger:
        name = "LongInteger";
        break;
      case AstarteDeviceSdk::AstarteType::kString:
        name = "String";
        break;
      case AstarteDeviceSdk::AstarteType::kBinaryBlobArray:
        name = "BinaryBlobArray";
        break;
      case AstarteDeviceSdk::AstarteType::kBooleanArray:
        name = "BooleanArray";
        break;
      case AstarteDeviceSdk::AstarteType::kDatetimeArray:
        name = "DatetimeArray";
        break;
      case AstarteDeviceSdk::AstarteType::kDoubleArray:
        name = "DoubleArray";
        break;
      case AstarteDeviceSdk::AstarteType::kIntegerArray:
        name = "IntegerArray";
        break;
      case AstarteDeviceSdk::AstarteType::kLongIntegerArray:
        name = "LongIntegerArray";
        break;
      case AstarteDeviceSdk::AstarteType::kStringArray:
        name = "StringArray";
        break;
    }

    return astarte_fmt::format_to(ctx.out(), "{}", name);
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::AstarteType typ)
    -> std::ostream& {
  out << astarte_fmt::format("{}", typ);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for
 * AstarteDeviceSdk::AstarteDatastreamIndividual.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::AstarteDatastreamIndividual> {
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
   * @brief Format the AstarteDatastreamIndividual object.
   * @param data The AstarteDatastreamIndividual to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteDatastreamIndividual& data, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", data.get_value());
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::AstarteDatastreamIndividual& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for
 * AstarteDeviceSdk::AstarteDatastreamObject.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::AstarteDatastreamObject> {
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
   * @brief Format the AstarteDatastreamObject object as a key-value map.
   * @param data The AstarteDatastreamObject to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteDatastreamObject& data, FormatContext& ctx) const {
    auto out = ctx.out();
    out = astarte_fmt::format_to(out, "{{");

    bool first = true;
    for (const auto& pair : data.get_raw_data()) {
      if (!first) {
        out = astarte_fmt::format_to(out, ", ");
      }
      out = astarte_fmt::format_to(out, R"("{}": {})", pair.first, pair.second);
      first = false;
    }

    out = astarte_fmt::format_to(out, "}}");
    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::AstarteDatastreamObject& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for
 * AstarteDeviceSdk::AstartePropertyIndividual.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::AstartePropertyIndividual> {
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
   * @brief Format the AstartePropertyIndividual object.
   * @param data The AstartePropertyIndividual to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstartePropertyIndividual& data, FormatContext& ctx) const {
    const auto& opt_data = data.get_value();
    if (opt_data.has_value()) {
      return astarte_fmt::format_to(ctx.out(), "{}", opt_data.value());
    }

    return ctx.out();
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::AstartePropertyIndividual& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for AstarteDeviceSdk::AstarteMessage.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::AstarteMessage> {
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
   * @brief Format the AstarteMessage object.
   * @param msg The AstarteMessage to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteMessage& msg, FormatContext& ctx) const {
    auto out = ctx.out();

    out = astarte_fmt::format_to(out, "{{interface: {}, path: {}", msg.get_interface(),
                                 msg.get_path());

    // check if the payload is an unset property, which is the only "empty" case
    bool is_unset_prop = false;
    const auto* prop =
        std::get_if<AstarteDeviceSdk::AstartePropertyIndividual>(&msg.get_raw_data());
    if (prop && !prop->get_value().has_value()) {
      is_unset_prop = true;
    }

    if (!is_unset_prop) {
      out = astarte_fmt::format_to(out, ", value: ");
      std::visit([&out](const auto& arg) { out = astarte_fmt::format_to(out, "{}", arg); },
                 msg.get_raw_data());
    }

    return astarte_fmt::format_to(out, "}}");
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::AstarteMessage& msg)
    -> std::ostream& {
  out << astarte_fmt::format("{}", msg);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for AstarteDeviceSdk::AstarteStoredProperty.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::AstarteStoredProperty> {
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
   * @brief Format the AstarteStoredProperty object.
   * @param msg The AstarteStoredProperty to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteStoredProperty& prop, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(),
                                  "Interface: {} v{}, Path: {}, Ownership: {}, Value: {}",
                                  prop.get_interface_name(), prop.get_version_major(),
                                  prop.get_path(), prop.get_ownership(), prop.get_value());
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::AstarteStoredProperty& prop)
    -> std::ostream& {
  out << astarte_fmt::format("{}", prop);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for AstarteDeviceSdk::AstarteStoredProperty.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::AstarteOwnership> {
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
   * @brief Format the AstarteDeviceSdk:: object.
   * @param msg The AstarteDeviceSdk:: to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteOwnership& ownership, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (ownership) {
      case AstarteDeviceSdk::AstarteOwnership::kDevice:
        astarte_fmt::format_to(out, "device");
        break;
      case AstarteDeviceSdk::AstarteOwnership::kServer:
        astarte_fmt::format_to(out, "server");
        break;
    }

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::AstarteOwnership ownership)
    -> std::ostream& {
  out << astarte_fmt::format("{}", ownership);
  return out;
}

/**
 * @brief Formatter specialization for AstarteDeviceSdk::AstarteErrorBase.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::AstarteErrorBase> {
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
   * @brief Format the AstarteErrorBase object.
   * @param err The AstarteErrorBase to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteErrorBase& err, FormatContext& ctx) const {
    auto out = astarte_fmt::format_to(ctx.out(), "{}: {}", err.type(), err.message());

    std::string indent;
    const AstarteDeviceSdk::AstarteErrorBase* current = &err;
    while (const auto& nested = current->nested_error()) {
      indent += "  ";
      out = astarte_fmt::format_to(out, "\n{}-> {}: {}", indent, nested->type(), nested->message());
      current = nested.get();
    }

    return out;
  }
};

/**
 * @brief Formatter specialization for AstarteDeviceSdk::AstarteError.
 */
template <>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct astarte_fmt::formatter<AstarteDeviceSdk::AstarteError> {
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
   * @brief Format the AstarteError object.
   * @param err_variant The AstarteError to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteError& err_variant, FormatContext& ctx) const {
    return std::visit(
        [&ctx](const auto& err) {
          const auto& base_err = static_cast<const AstarteDeviceSdk::AstarteErrorBase&>(err);
          return astarte_fmt::format_to(ctx.out(), "{}", base_err);
        },
        err_variant);
  }
};

/// @endcond

#endif  // ASTARTE_FORMATTER_H
