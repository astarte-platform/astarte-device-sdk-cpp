// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_FORMATTER_H
#define ASTARTE_FORMATTER_H

#include <chrono>
#include <cstddef>
#include <type_traits>

// TODO(rgallor): stop using spdlog formatter once C++20 will become the minimu required version
#if (__cplusplus >= 202002L) && (__has_include(<format>))
#include <format>
#define NS_FORMAT std
#else                        // (__cplusplus >= 202002L) && (__has_include(<format>))
#include <spdlog/fmt/fmt.h>  // NOLINT: avoid clang-tidy warning regarding fmt library not used directly

#include <iomanip>
#include <sstream>
#define NS_FORMAT fmt
#endif  // (__cplusplus >= 202002L) && (__has_include(<format>))

#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/type.hpp"

/**
 * @brief Contains utility functions for formatting data.
 */
namespace utils {
// These functions are only used for pretty printing
// NOLINTBEGIN(concurrency-mt-unsafe)
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
/**
 * @brief Formats a vector of bytes into a Base64 string literal.
 * @tparam OutputIt The type of the output iterator.
 * @param out Reference to the output iterator where the result is written.
 * @param data The vector of bytes to format.
 */
template <typename OutputIt>
void format_base64(OutputIt &out, const std::vector<uint8_t> &data) {
  static constexpr std::string_view base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/";

  size_t idx = 0;
  const size_t len = data.size();

  out = NS_FORMAT::format_to(out, "\"");

  while (idx + 2 < len) {
    const uint32_t chunk = (data[idx] << 16) | (data[idx + 1] << 8) | data[idx + 2];
    out = NS_FORMAT::format_to(out, "{}{}{}{}", base64_chars[(chunk >> 18) & 0x3F],
                               base64_chars[(chunk >> 12) & 0x3F],
                               base64_chars[(chunk >> 6) & 0x3F], base64_chars[chunk & 0x3F]);
    idx += 3;
  }

  if (idx < len) {
    uint32_t chunk = data[idx] << 16;
    if (idx + 1 < len) {
      chunk |= data[idx + 1] << 8;
    }

    out = NS_FORMAT::format_to(out, "{}{}", base64_chars[(chunk >> 18) & 0x3F],
                               base64_chars[(chunk >> 12) & 0x3F]);
    if (idx + 1 < len) {
      out = NS_FORMAT::format_to(out, "{}=", base64_chars[(chunk >> 6) & 0x3F]);
    } else {
      out = NS_FORMAT::format_to(out, "==");
    }
  }

  out = NS_FORMAT::format_to(out, "\"");
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

/**
 * @brief Formats a timestamp into an ISO 8601 string literal.
 * @tparam OutputIt The type of the output iterator.
 * @param out Reference to the output iterator where the result is written.
 * @param data The time_point to format.
 */
template <typename OutputIt>
void format_timestamp(OutputIt &out, const std::chrono::system_clock::time_point &data) {
  out = NS_FORMAT::format_to(out, "\"");
#if (__cplusplus >= 202002L) && (__has_include(<format>))
  out = NS_FORMAT::format_to(
      out, "{}",
      NS_FORMAT::format("{0:%F}T{0:%T}Z",
                        std::chrono::time_point_cast<std::chrono::milliseconds>(data)));
#else   // (__cplusplus >= 202002L) && (__has_include(<format>))
  const std::time_t time = std::chrono::system_clock::to_time_t(data);
  const std::tm utc_tm = *std::gmtime(&time);
  std::stringstream stream;
  stream << std::put_time(&utc_tm, "%FT%T.000Z");
  out = NS_FORMAT::format_to(out, "{}", stream.str());
#endif  // (__cplusplus >= 202002L) && (__has_include(<format>))
  out = NS_FORMAT::format_to(out, "\"");
}

/**
 * @brief Formats a generic data type into an output iterator.
 * @tparam OutputIt The type of the output iterator.
 * @tparam T The type of the element.
 * @param out Reference to the output iterator where the result is written.
 * @param data The element to format.
 */
template <typename OutputIt, typename T>
void format_data(OutputIt &out, const T &data) {
  if constexpr (std::is_same_v<T, bool>) {
    NS_FORMAT::format_to(out, "{}", (data ? "true" : "false"));
  } else if constexpr (std::is_same_v<T, std::string>) {
    NS_FORMAT::format_to(out, "\"{}\"", data);
  } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
    format_base64(out, data);
  } else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {
    format_timestamp(out, data);
  } else {  // default format case
    NS_FORMAT::format_to(out, "{}", data);
  }
}

/**
 * @brief Formats a generic vector into a comma-separated list in brackets.
 * @tparam OutputIt The type of the output iterator.
 * @tparam T The type of elements in the vector.
 * @param out Reference to the output iterator where the result is written.
 * @param data The vector to format.
 */
template <typename OutputIt, typename T>
void format_vector(OutputIt &out, const std::vector<T> &data) {
  out = NS_FORMAT::format_to(out, "[");
  for (size_t i = 0; i < data.size(); ++i) {
    format_data(out, data[i]);
    if (i != data.size() - 1) {
      out = NS_FORMAT::format_to(out, ", ");
    }
  }
  out = NS_FORMAT::format_to(out, "]");
}
// NOLINTEND(concurrency-mt-unsafe)

}  // namespace utils

/// @cond Doxygen should skip checking NS_FORMAT::formatter due to internal inconsistency parsing

/**
 * @brief NS_FORMAT::formatter specialization for AstarteDeviceSdk::AstarteData.
 */
template <>
struct NS_FORMAT::formatter<AstarteDeviceSdk::AstarteData> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the AstarteData variant-like object by dispatching to the correct formatter.
   * @param data The AstarteData to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteData &data, FormatContext &ctx) const {
    auto out = ctx.out();

    if (std::holds_alternative<int32_t>(data.get_raw_data())) {
      out = NS_FORMAT::format_to(out, "{}", std::get<int32_t>(data.get_raw_data()));
    } else if (std::holds_alternative<int64_t>(data.get_raw_data())) {
      out = NS_FORMAT::format_to(out, "{}", std::get<int64_t>(data.get_raw_data()));
    } else if (std::holds_alternative<double>(data.get_raw_data())) {
      out = NS_FORMAT::format_to(out, "{}", std::get<double>(data.get_raw_data()));
    } else if (std::holds_alternative<bool>(data.get_raw_data())) {
      auto s = (std::get<bool>(data.get_raw_data()) ? "true" : "false");
      out = NS_FORMAT::format_to(out, "{}", s);
    } else if (std::holds_alternative<std::string>(data.get_raw_data())) {
      out = NS_FORMAT::format_to(out, "\"{}\"", std::get<std::string>(data.get_raw_data()));
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

/**
 * @brief NS_FORMAT::formatter specialization for AstarteDeviceSdk::AstarteType.
 */
template <>
struct NS_FORMAT::formatter<AstarteDeviceSdk::AstarteType> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the AstarteType enum to its string representation.
   * @param typ The AstarteType to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteType &typ, FormatContext &ctx) const {
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

    return NS_FORMAT::format_to(ctx.out(), "{}", name);
  }
};

/**
 * @brief NS_FORMAT::formatter specialization for AstarteDeviceSdk::AstarteDatastreamIndividual.
 */
template <>
struct NS_FORMAT::formatter<AstarteDeviceSdk::AstarteDatastreamIndividual> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the AstarteDatastreamIndividual object.
   * @param data The AstarteDatastreamIndividual to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteDatastreamIndividual &data, FormatContext &ctx) const {
    return NS_FORMAT::format_to(ctx.out(), "{}", data.get_value());
  }
};

/**
 * @brief NS_FORMAT::formatter specialization for AstarteDeviceSdk::AstarteDatastreamObject.
 */
template <>
struct NS_FORMAT::formatter<AstarteDeviceSdk::AstarteDatastreamObject> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the AstarteDatastreamObject object as a key-value map.
   * @param data The AstarteDatastreamObject to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteDatastreamObject &data, FormatContext &ctx) const {
    auto out = ctx.out();
    out = NS_FORMAT::format_to(out, "{{");

    bool first = true;
    for (const auto &pair : data.get_raw_data()) {
      if (!first) {
        out = NS_FORMAT::format_to(out, ", ");
      }
      out = NS_FORMAT::format_to(out, "\"{}\": {}", pair.first, pair.second);
      first = false;
    }

    out = NS_FORMAT::format_to(out, "}}");
    return out;
  }
};

/**
 * @brief NS_FORMAT::formatter specialization for AstarteDeviceSdk::AstartePropertyIndividual.
 */
template <>
struct NS_FORMAT::formatter<AstarteDeviceSdk::AstartePropertyIndividual> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the AstartePropertyIndividual object.
   * @param data The AstartePropertyIndividual to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstartePropertyIndividual &data, FormatContext &ctx) const {
    if (data.get_value().has_value()) {
      return NS_FORMAT::format_to(ctx.out(), "{}", data.get_value().value());
    }

    return ctx.out();
  }
};

/**
 * @brief NS_FORMAT::formatter specialization for AstarteDeviceSdk::AstarteMessage.
 */
template <>
struct NS_FORMAT::formatter<AstarteDeviceSdk::AstarteMessage> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the AstarteMessage object.
   * @param msg The AstarteMessage to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteMessage &msg, FormatContext &ctx) const {
    auto out = ctx.out();

    out =
        NS_FORMAT::format_to(out, "{{interface: {}, path: {}", msg.get_interface(), msg.get_path());

    // check if the payload is an unset property, which is the only "empty" case
    bool is_unset_prop = false;
    if (const auto *prop =
            std::get_if<AstarteDeviceSdk::AstartePropertyIndividual>(&msg.get_raw_data())) {
      if (!prop->get_value().has_value()) {
        is_unset_prop = true;
      }
    }

    if (!is_unset_prop) {
      out = NS_FORMAT::format_to(out, ", value: ");
      std::visit([&out](const auto &arg) { out = NS_FORMAT::format_to(out, "{}", arg); },
                 msg.get_raw_data());
    }

    return NS_FORMAT::format_to(out, "}}");
  }
};

/// @endcond

#endif  // ASTARTE_FORMATTER_H
