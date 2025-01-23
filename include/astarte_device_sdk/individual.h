// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_INDIVIDUAL_H
#define ASTARTE_DEVICE_SDK_INDIVIDUAL_H

/**
 * @file astarte_device_sdk/individual.h
 * @brief Astarte individual object and its related methods.
 */

#include <chrono>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#if !(__cplusplus >= 202002L)
#include <type_traits>
#endif  // !(__cplusplus >= 202002L)

namespace AstarteDeviceSdk {

/** @brief Astarte individual object, used for transmission of individual data streams. */
using AstarteIndividual =
    std::variant<int32_t, int64_t, double, bool, std::string, std::vector<uint8_t>,
                 std::chrono::system_clock::time_point, std::vector<int32_t>, std::vector<int64_t>,
                 std::vector<double>, std::vector<bool>, std::vector<std::string>,
                 std::vector<std::vector<uint8_t>>,
                 std::vector<std::chrono::system_clock::time_point>>;

#if __cplusplus >= 202002L
/** @brief Restricts the allowed types for the creation fo an Astarte individual object. */
template <typename T>
concept AstarteIndividualAllowedType = requires {
  requires std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t> || std::is_same_v<T, double> ||
               std::is_same_v<T, bool> || std::is_same_v<T, std::string> ||
               std::is_same_v<T, std::vector<uint8_t>> ||
               std::is_same_v<T, std::chrono::system_clock::time_point> ||
               std::is_same_v<T, std::vector<int32_t>> || std::is_same_v<T, std::vector<int64_t>> ||
               std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<bool>> ||
               std::is_same_v<T, std::vector<std::string>> ||
               std::is_same_v<T, std::vector<std::vector<uint8_t>>> ||
               std::is_same_v<T, std::vector<std::chrono::system_clock::time_point>>;
};

/**
 * @brief Conversion function for any of the allowed types into an Astarte individual.
 * @param value Value to convert into an Astarte individual.
 * @return The converted Astarte individual.
 */
template <AstarteIndividualAllowedType T>
auto to_astarte_individual(T value) -> AstarteIndividual {
  return value;
}
#else   // __cplusplus >= 202002L
template <typename T>
struct astarte_individual_is_allowed_type : std::false_type {};
template <>
struct astarte_individual_is_allowed_type<int32_t> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<int64_t> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<double> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<bool> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::string> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<uint8_t>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::chrono::system_clock::time_point> : std::true_type {
};
template <>
struct astarte_individual_is_allowed_type<std::vector<int32_t>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<int64_t>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<double>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<bool>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<std::string>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<std::vector<uint8_t>>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<std::chrono::system_clock::time_point>>
    : std::true_type {};

/**
 * @brief Conversion function for any of the allowed types into an Astarte individual.
 * @param value Value to convert into an Astarte individual.
 * @return The converted Astarte individual.
 */
template <typename T>
auto to_astarte_individual(T value)
    -> std::enable_if_t<astarte_individual_is_allowed_type<T>::value, AstarteIndividual> {
  return value;
}
#endif  // __cplusplus >= 202002L

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_INDIVIDUAL_H
