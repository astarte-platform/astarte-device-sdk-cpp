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
 * @param typ The string representation of the astarte type.
 * @return The corresponding AstarteType enum value, an error if the string is not a valid Astarte
 * type.
 */
// TODO: use static or move function definition to .cpp file
inline auto astarte_type_from_str(std::string typ)
    -> astarte_tl::expected<AstarteType, AstarteError> {
  if (typ == "binaryblob") {
    return AstarteType::kBinaryBlob;
  } else if (typ == "boolean") {
    return AstarteType::kBoolean;
  } else if (typ == "datetime") {
    return AstarteType::kDatetime;
  } else if (typ == "double") {
    return AstarteType::kDouble;
  } else if (typ == "integer") {
    return AstarteType::kInteger;
  } else if (typ == "longinteger") {
    return AstarteType::kLongInteger;
  } else if (typ == "string") {
    return AstarteType::kString;
  } else if (typ == "binaryblobarray") {
    return AstarteType::kBinaryBlobArray;
  } else if (typ == "booleanarray") {
    return AstarteType::kBooleanArray;
  } else if (typ == "datetimearray") {
    return AstarteType::kDatetimeArray;
  } else if (typ == "doublearray") {
    return AstarteType::kDoubleArray;
  } else if (typ == "integerarray") {
    return AstarteType::kIntegerArray;
  } else if (typ == "longintegerarray") {
    return AstarteType::kLongIntegerArray;
  } else if (typ == "stringarray") {
    return AstarteType::kStringArray;
  } else {
    return astarte_tl::unexpected(
        AstarteInvalidAstarteTypeError("interface ownershipe not valid: " + typ));
  }

  // InvalidMappingTypeException
}

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_TYPE_H
