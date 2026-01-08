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
inline auto astarte_type_from_str(std::string type)
    -> astarte_tl::expected<AstarteType, AstarteError> {
  if (type == "binaryblob") {
    return AstarteType::kBinaryBlob;
  } else if (type == "boolean") {
    return AstarteType::kBoolean;
  } else if (type == "datetime") {
    return AstarteType::kDatetime;
  } else if (type == "double") {
    return AstarteType::kDouble;
  } else if (type == "integer") {
    return AstarteType::kInteger;
  } else if (type == "longinteger") {
    return AstarteType::kLongInteger;
  } else if (type == "string") {
    return AstarteType::kString;
  } else if (type == "binaryblobarray") {
    return AstarteType::kBinaryBlobArray;
  } else if (type == "booleanarray") {
    return AstarteType::kBooleanArray;
  } else if (type == "datetimearray") {
    return AstarteType::kDatetimeArray;
  } else if (type == "doublearray") {
    return AstarteType::kDoubleArray;
  } else if (type == "integerarray") {
    return AstarteType::kIntegerArray;
  } else if (type == "longintegerarray") {
    return AstarteType::kLongIntegerArray;
  } else if (type == "stringarray") {
    return AstarteType::kStringArray;
  } else {
    return astarte_tl::unexpected(AstarteInvalidAstarteTypeError("data type not valid: " + type));
  }
}

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_TYPE_H
