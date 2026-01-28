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

namespace AstarteDeviceSdk {

/** @brief Possible Astarte ownership. */
enum AstarteOwnership : int8_t {
  /** @brief Ownership is retained by the device. */
  kDevice,
  /** @brief Ownership is retained by the Astarte cloud server. */
  kServer
};

/**
 * @brief Convert a string to an AstarteOwnership enum.
 *
 * @param ownership The string representation of the interface ownership.
 * @return The corresponding AstarteOwnership enum value, an error if the string is not a valid
 * ownership.
 */
inline auto ownership_from_str(const std::string& ownership)
    -> astarte_tl::expected<AstarteOwnership, AstarteError> {
  if (ownership == "device") {
    return AstarteOwnership::kDevice;
  }
  if (ownership == "server") {
    return AstarteOwnership::kServer;
  }
  return astarte_tl::unexpected(
      AstarteInvalidInterfaceOwnershipeError("interface ownershipe not valid: " + ownership));
}

}  // namespace AstarteDeviceSdk

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
   * @param ownership The AstarteDeviceSdk:: to format.
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

/**
 * @brief Stream insertion operator for AstarteOwnership.
 * @param out The output stream.
 * @param ownership The AstarteOwnership enum to output.
 * @return Reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::AstarteOwnership ownership)
    -> std::ostream& {
  out << astarte_fmt::format("{}", ownership);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_OWNERSHIP_H
