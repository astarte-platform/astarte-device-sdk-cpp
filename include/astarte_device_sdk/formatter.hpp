// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_FORMATTER_H
#define ASTARTE_FORMATTER_H

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) ||    \
     (!defined(_MSVC_LANG) && __cplusplus >= 202002L)) && \
    (__has_include(<format>))

#include <format>
namespace astarte_fmt = ::std;

#else

#include <spdlog/fmt/fmt.h>

#include <iomanip>
#include <sstream>
namespace astarte_fmt = ::fmt;

#endif

#ifdef DOXYGEN
/** @brief Namespace alias for the formatting library (std or fmt). */
namespace astarte_fmt {
/** @brief Base formatter struct declaration for Doxygen. */
template <typename T>
struct formatter {};
}  // namespace astarte_fmt
#endif

/** @brief Utility functions for formatting data. */
namespace AstarteDeviceSdk::utils {

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

  // Named constants to avoid magic numbers
  static constexpr uint32_t mask_6bit = 0x3F;
  static constexpr int shift_byte_0 = 16;
  static constexpr int shift_byte_1 = 8;
  static constexpr int shift_b64_0 = 18;
  static constexpr int shift_b64_1 = 12;
  static constexpr int shift_b64_2 = 6;

  size_t idx = 0;
  const size_t len = data.size();

  out = astarte_fmt::format_to(out, "\"");

  while (idx + 2 < len) {
    const uint32_t chunk =
        (data[idx] << shift_byte_0) | (data[idx + 1] << shift_byte_1) | data[idx + 2];
    out = astarte_fmt::format_to(out, "{}{}{}{}", base64_chars[(chunk >> shift_b64_0) & mask_6bit],
                                 base64_chars[(chunk >> shift_b64_1) & mask_6bit],
                                 base64_chars[(chunk >> shift_b64_2) & mask_6bit],
                                 base64_chars[chunk & mask_6bit]);
    idx += 3;
  }

  if (idx < len) {
    uint32_t chunk = data[idx] << shift_byte_0;
    if (idx + 1 < len) {
      chunk |= data[idx + 1] << shift_byte_1;
    }

    out = astarte_fmt::format_to(out, "{}{}", base64_chars[(chunk >> shift_b64_0) & mask_6bit],
                                 base64_chars[(chunk >> shift_b64_1) & mask_6bit]);
    if (idx + 1 < len) {
      out = astarte_fmt::format_to(out, "{}=", base64_chars[(chunk >> shift_b64_2) & mask_6bit]);
    } else {
      out = astarte_fmt::format_to(out, "==");
    }
  }

  out = astarte_fmt::format_to(out, "\"");
}

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

}  // namespace AstarteDeviceSdk::utils

#endif  // ASTARTE_FORMATTER_H
