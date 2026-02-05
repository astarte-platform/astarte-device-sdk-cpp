// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_OWNERSHIP_H
#define ASTARTE_DEVICE_SDK_OWNERSHIP_H

/**
 * @file astarte_device_sdk/ownership.hpp
 * @brief Ownership definitions for communication with Astarte.
 */

#include <cstdint>
#include <ostream>
#include <string>

#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

/// @brief Possible Astarte ownership.
enum Ownership : int8_t {
  /// @brief Ownership is retained by the device.
  kDevice,
  /// @brief Ownership is retained by the Astarte cloud server.
  kServer
};

/**
 * @brief Converts a string to an Ownership enum.
 *
 * @param[in] ownership The string representation of the interface ownership.
 * @return An expected containing the Ownership on success or Error on failure.
 */
inline auto ownership_from_str(const std::string& ownership)
    -> astarte_tl::expected<Ownership, Error> {
  if (ownership == "device") {
    return Ownership::kDevice;
  }
  if (ownership == "server") {
    return Ownership::kServer;
  }
  return astarte_tl::unexpected(
      InvalidInterfaceOwnershipeError("interface ownership not valid: " + ownership));
}

}  // namespace astarte::device

/// @brief astarte_fmt::formatter specialization for astarte::device::Ownership.
template <>
struct astarte_fmt::formatter<astarte::device::Ownership> {
  /**
   * @brief Parses the format string. Default implementation.
   *
   * @tparam ParseContext The type of the parse context.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the astarte::device::Ownership object.
   *
   * @tparam FormatContext The type of the format context.
   * @param[in] ownership The astarte::device::Ownership to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::Ownership& ownership, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (ownership) {
      case astarte::device::Ownership::kDevice:
        astarte_fmt::format_to(out, "device");
        break;
      case astarte::device::Ownership::kServer:
        astarte_fmt::format_to(out, "server");
        break;
    }

    return out;
  }
};

/**
 * @brief Stream insertion operator for Ownership.
 *
 * @param[in,out] out The output stream.
 * @param[in] ownership The Ownership enum to output.
 * @return A reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::Ownership ownership)
    -> std::ostream& {
  out << astarte_fmt::format("{}", ownership);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_OWNERSHIP_H
