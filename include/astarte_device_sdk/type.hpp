// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_TYPE_H
#define ASTARTE_DEVICE_SDK_TYPE_H

/**
 * @file astarte_device_sdk/type.hpp
 * @brief Types definitions for communication with Astarte.
 *
 * @details This file defines the enumeration of supported Astarte data types and provides
 * utility functions for string conversion and formatting, enabling type-safe data
 * handling within the SDK.
 */

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

/**
 * @brief Enumeration of supported Astarte data types.
 *
 * @details These types correspond to the data types defined in Astarte interfaces
 * and are used to validate and tag data sent to or received from the platform.
 */
enum Type : int8_t {
  /// @brief Binary blob Astarte type.
  kBinaryBlob,
  /// @brief Boolean Astarte type.
  kBoolean,
  /// @brief Date-time Astarte type.
  kDatetime,
  /// @brief Double Astarte type.
  kDouble,
  /// @brief Integer Astarte type.
  kInteger,
  /// @brief Long integer Astarte type.
  kLongInteger,
  /// @brief String Astarte type.
  kString,
  /// @brief Binary blob array Astarte type.
  kBinaryBlobArray,
  /// @brief Boolean array Astarte type.
  kBooleanArray,
  /// @brief Datetime array Astarte type.
  kDatetimeArray,
  /// @brief Double array Astarte type.
  kDoubleArray,
  /// @brief Integer array Astarte type.
  kIntegerArray,
  /// @brief Long integer array Astarte type.
  kLongIntegerArray,
  /// @brief String array Astarte type.
  kStringArray
};

/**
 * @brief Converts a string representation of a type to a Type enum.
 *
 * @details This function maps the string identifiers used in Astarte interface
 * definitions (e.g., "binaryblob", "integer") to their corresponding C++ enum values.
 *
 * @param[in] type The string representation of the Astarte type.
 * @return An expected containing the Type on success or Error on failure.
 */
// NOLINTNEXTLINE(readability-function-size)
inline auto astarte_type_from_str(const std::string& type) -> astarte_tl::expected<Type, Error> {
  if (type == "binaryblob") {
    return Type::kBinaryBlob;
  }
  if (type == "boolean") {
    return Type::kBoolean;
  }
  if (type == "datetime") {
    return Type::kDatetime;
  }
  if (type == "double") {
    return Type::kDouble;
  }
  if (type == "integer") {
    return Type::kInteger;
  }
  if (type == "longinteger") {
    return Type::kLongInteger;
  }
  if (type == "string") {
    return Type::kString;
  }
  if (type == "binaryblobarray") {
    return Type::kBinaryBlobArray;
  }
  if (type == "booleanarray") {
    return Type::kBooleanArray;
  }
  if (type == "datetimearray") {
    return Type::kDatetimeArray;
  }
  if (type == "doublearray") {
    return Type::kDoubleArray;
  }
  if (type == "integerarray") {
    return Type::kIntegerArray;
  }
  if (type == "longintegerarray") {
    return Type::kLongIntegerArray;
  }
  if (type == "stringarray") {
    return Type::kStringArray;
  }
  return astarte_tl::unexpected(InvalidAstarteTypeError("data type not valid: " + type));
}

}  // namespace astarte::device

/// @brief astarte_fmt::formatter specialization for astarte::device::Type.
template <>
struct astarte_fmt::formatter<astarte::device::Type> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param[in] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the Type enum to its string representation.
   * @param[in] typ The Type to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::Type& typ, FormatContext& ctx) const {
    std::string_view name = "Unknown Type";

    switch (typ) {
      case astarte::device::Type::kBinaryBlob:
        name = "BinaryBlob";
        break;
      case astarte::device::Type::kBoolean:
        name = "Boolean";
        break;
      case astarte::device::Type::kDatetime:
        name = "Datetime";
        break;
      case astarte::device::Type::kDouble:
        name = "Double";
        break;
      case astarte::device::Type::kInteger:
        name = "Integer";
        break;
      case astarte::device::Type::kLongInteger:
        name = "LongInteger";
        break;
      case astarte::device::Type::kString:
        name = "String";
        break;
      case astarte::device::Type::kBinaryBlobArray:
        name = "BinaryBlobArray";
        break;
      case astarte::device::Type::kBooleanArray:
        name = "BooleanArray";
        break;
      case astarte::device::Type::kDatetimeArray:
        name = "DatetimeArray";
        break;
      case astarte::device::Type::kDoubleArray:
        name = "DoubleArray";
        break;
      case astarte::device::Type::kIntegerArray:
        name = "IntegerArray";
        break;
      case astarte::device::Type::kLongIntegerArray:
        name = "LongIntegerArray";
        break;
      case astarte::device::Type::kStringArray:
        name = "StringArray";
        break;
    }

    return astarte_fmt::format_to(ctx.out(), "{}", name);
  }
};

/**
 * @brief Stream insertion operator for Type.
 *
 * @param[in,out] out The output stream to write to.
 * @param[in] typ The Type enum to output.
 * @return A reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::Type typ) -> std::ostream& {
  out << astarte_fmt::format("{}", typ);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_TYPE_H
