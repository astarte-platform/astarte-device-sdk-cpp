// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_TYPE_H
#define ASTARTE_DEVICE_SDK_TYPE_H

/**
 * @file astarte_device_sdk/type.hpp
 * @brief Types definitions for communication with Astarte.
 */

#include <astarte_device_sdk/errors.hpp>
#include <cstdint>
#include <string>

namespace AstarteDeviceSdk {

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
    -> astarte_tl::expected<AstarteType, AstarteError> {
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
  return astarte_tl::unexpected(AstarteInvalidAstarteTypeError("data type not valid: " + type));
}

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_TYPE_H
