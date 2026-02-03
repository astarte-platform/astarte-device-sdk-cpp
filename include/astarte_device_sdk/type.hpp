// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_TYPE_H
#define ASTARTE_DEVICE_SDK_TYPE_H

/**
 * @file astarte_device_sdk/type.hpp
 * @brief Types definitions for communication with Astarte.
 */

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

/** @brief Possible Astarte types. */
enum AstarteType : int8_t {
  /** @brief Binary blob Astarte type. */
  kBinaryBlob,
  /** @brief Boolean Astarte type. */
  kBoolean,
  /** @brief Date-time Astarte type. */
  kDatetime,
  /** @brief Double Astarte type. */
  kDouble,
  /** @brief Integer Astarte type. */
  kInteger,
  /** @brief Long integer Astarte type. */
  kLongInteger,
  /** @brief String Astarte type. */
  kString,
  /** @brief Binary blob array Astarte type. */
  kBinaryBlobArray,
  /** @brief Boolean array Astarte type. */
  kBooleanArray,
  /** @brief Datetime array Astarte type. */
  kDatetimeArray,
  /** @brief Double array Astarte type. */
  kDoubleArray,
  /** @brief Integer array Astarte type. */
  kIntegerArray,
  /** @brief Long integer array Astarte type. */
  kLongIntegerArray,
  /** @brief String array Astarte type. */
  kStringArray
};

/**
 * @brief Convert a string to an AstarteType enum.
 *
 * @param type The string representation of the astarte type.
 * @return The corresponding AstarteType enum value, an error if the string is not a valid Astarte
 * type.
 */
// NOLINTNEXTLINE(readability-function-size)
inline auto astarte_type_from_str(const std::string& type)
    -> astarte_tl::expected<AstarteType, Error> {
  if (type == "binaryblob") {
    return AstarteType::kBinaryBlob;
  }
  if (type == "boolean") {
    return AstarteType::kBoolean;
  }
  if (type == "datetime") {
    return AstarteType::kDatetime;
  }
  if (type == "double") {
    return AstarteType::kDouble;
  }
  if (type == "integer") {
    return AstarteType::kInteger;
  }
  if (type == "longinteger") {
    return AstarteType::kLongInteger;
  }
  if (type == "string") {
    return AstarteType::kString;
  }
  if (type == "binaryblobarray") {
    return AstarteType::kBinaryBlobArray;
  }
  if (type == "booleanarray") {
    return AstarteType::kBooleanArray;
  }
  if (type == "datetimearray") {
    return AstarteType::kDatetimeArray;
  }
  if (type == "doublearray") {
    return AstarteType::kDoubleArray;
  }
  if (type == "integerarray") {
    return AstarteType::kIntegerArray;
  }
  if (type == "longintegerarray") {
    return AstarteType::kLongIntegerArray;
  }
  if (type == "stringarray") {
    return AstarteType::kStringArray;
  }
  return astarte_tl::unexpected(InvalidAstarteTypeError("data type not valid: " + type));
}

}  // namespace astarte::device

/**
 * @brief astarte_fmt::formatter specialization for astarte::device::AstarteType.
 */
template <>
struct astarte_fmt::formatter<astarte::device::AstarteType> {
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
  auto format(const astarte::device::AstarteType& typ, FormatContext& ctx) const {
    std::string_view name = "Unknown Type";

    switch (typ) {
      case astarte::device::AstarteType::kBinaryBlob:
        name = "BinaryBlob";
        break;
      case astarte::device::AstarteType::kBoolean:
        name = "Boolean";
        break;
      case astarte::device::AstarteType::kDatetime:
        name = "Datetime";
        break;
      case astarte::device::AstarteType::kDouble:
        name = "Double";
        break;
      case astarte::device::AstarteType::kInteger:
        name = "Integer";
        break;
      case astarte::device::AstarteType::kLongInteger:
        name = "LongInteger";
        break;
      case astarte::device::AstarteType::kString:
        name = "String";
        break;
      case astarte::device::AstarteType::kBinaryBlobArray:
        name = "BinaryBlobArray";
        break;
      case astarte::device::AstarteType::kBooleanArray:
        name = "BooleanArray";
        break;
      case astarte::device::AstarteType::kDatetimeArray:
        name = "DatetimeArray";
        break;
      case astarte::device::AstarteType::kDoubleArray:
        name = "DoubleArray";
        break;
      case astarte::device::AstarteType::kIntegerArray:
        name = "IntegerArray";
        break;
      case astarte::device::AstarteType::kLongIntegerArray:
        name = "LongIntegerArray";
        break;
      case astarte::device::AstarteType::kStringArray:
        name = "StringArray";
        break;
    }

    return astarte_fmt::format_to(ctx.out(), "{}", name);
  }
};

/**
 * @brief Stream insertion operator for AstarteType.
 * @param out The output stream.
 * @param typ The AstarteType enum to output.
 * @return Reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::AstarteType typ) -> std::ostream& {
  out << astarte_fmt::format("{}", typ);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_TYPE_H
