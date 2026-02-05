// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_DATA_H
#define ASTARTE_DEVICE_SDK_DATA_H

/**
 * @file astarte_device_sdk/data.hpp
 * @brief Astarte data class and its related methods.
 *
 * @details This file defines the container class for data exchanged with the Astarte
 * platform, handling type mapping between C++ and Astarte's data model.
 */

#include <chrono>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/type.hpp"

namespace astarte::device {

/**
 * @brief C++ concept to restrict the allowed types for instances of an Astarte data class.
 *
 * @details This concept ensures that only specific C++ types, corresponding to Astarte's
 * supported data types, can be used to construct and retrieve data from the `Data` class.
 */
template <typename T>
concept DataAllowedType = requires {
  requires std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t> || std::is_same_v<T, double> ||
               std::is_same_v<T, bool> || std::is_same_v<T, std::string> ||
               std::is_same_v<T, std::string_view> || std::is_same_v<T, std::vector<uint8_t>> ||
               std::is_same_v<T, std::chrono::system_clock::time_point> ||
               std::is_same_v<T, std::vector<int32_t>> || std::is_same_v<T, std::vector<int64_t>> ||
               std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<bool>> ||
               std::is_same_v<T, std::vector<std::string>> ||
               std::is_same_v<T, std::vector<std::vector<uint8_t>>> ||
               std::is_same_v<T, std::vector<std::chrono::system_clock::time_point>>;
};

/**
 * @brief Represents a single Astarte data value.
 *
 * @details The `Data` class acts as a versatile container for all data types supported by the
 * Astarte platform. It abstracts away the underlying C++ types, providing a unified interface
 * for handling data sent to and received from Astarte.
 *
 * It uses `std::variant` internally to store different types, ensuring type safety
 * while offering flexibility. The `DataAllowedType` concept restricts which C++ types
 * can be stored, mapping them to Astarte's data model.
 */
class Data {
 public:
  /**
   * @brief Constructs a Data object from a value of any `DataAllowedType`.
   *
   * @details The constructor implicitly copies the provided value into the internal storage.
   * This ensures that the `Data` object manages its own data lifetime independently
   * of the input `value`.
   *
   * @tparam T The type of the value, must satisfy `DataAllowedType`.
   * @param[in] value The content to initialize the Astarte data instance with.
   */
  template <DataAllowedType T>
  explicit Data(const T& value) {
    if constexpr (std::is_same_v<T, std::string_view>) {
      data_ = std::string(value);
    } else {
      data_ = value;
    }
  }

  /**
   * @brief Converts the Astarte data class to the specified type `T`.
   *
   * @details This method performs a direct extraction of the stored value using `std::get<T>`.
   * It is the caller's responsibility to ensure that the `Data` object
   * currently holds a value of type `T`.
   *
   * @tparam T The target type to convert to, must satisfy `DataAllowedType`.
   * @return A constant reference to the value contained in the class instance.
   * @throws std::bad_variant_access If the `Data` object does not contain a `T`.
   */
  template <DataAllowedType T>
  [[nodiscard]] auto into() const
      -> std::conditional_t<std::is_same_v<T, std::string_view>, std::string_view, const T&> {
    if constexpr (std::is_same_v<T, std::string_view>) {
      return std::string_view(std::get<std::string>(data_));
    } else {
      return std::get<T>(data_);
    }
  }

  /**
   * @brief Attempts to convert the Astarte data class to the specified type `T` safely.
   *
   * @details This method attempts to extract the stored value as type `T`. If the `Data`
   * object currently holds a value of type `T`, an `std::optional` containing
   * that value is returned.
   *
   * @tparam T The target type to convert to, must satisfy `DataAllowedType`.
   * @return std::optional containing the value if the types match, or std::nullopt otherwise.
   */
  template <DataAllowedType T>
  [[nodiscard]] auto try_into() const -> std::optional<T> {
    if constexpr (std::is_same_v<T, std::string_view>) {
      if (std::holds_alternative<std::string>(data_)) {
        return std::string_view(std::get<std::string>(data_));
      }
    } else {
      if (std::holds_alternative<T>(data_)) {
        return std::get<T>(data_);
      }
    }

    return std::nullopt;
  }

  /**
   * @brief Gets the type of the data contained in this class instance.
   * @return The Astarte Type of the content of this class instance.
   */
  [[nodiscard]] auto get_type() const -> Type;

  /**
   * @brief Returns the raw data variant contained in this class instance.
   * @return A constant reference to the internal variant containing one of the possible data types.
   */
  [[nodiscard]] auto get_raw_data() const
      -> const std::variant<int32_t, int64_t, double, bool, std::string, std::vector<uint8_t>,
                            std::chrono::system_clock::time_point, std::vector<int32_t>,
                            std::vector<int64_t>, std::vector<double>, std::vector<bool>,
                            std::vector<std::string>, std::vector<std::vector<uint8_t>>,
                            std::vector<std::chrono::system_clock::time_point>>&;
  /**
   * @brief Overloader for the comparison operator ==.
   * @param[in] other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const Data& other) const -> bool;

  /**
   * @brief Overloader for the comparison operator !=.
   * @param[in] other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const Data& other) const -> bool;

 private:
  std::variant<int32_t, int64_t, double, bool, std::string, std::vector<uint8_t>,
               std::chrono::system_clock::time_point, std::vector<int32_t>, std::vector<int64_t>,
               std::vector<double>, std::vector<bool>, std::vector<std::string>,
               std::vector<std::vector<uint8_t>>,
               std::vector<std::chrono::system_clock::time_point>>
      data_;
};

}  // namespace astarte::device

/// @brief astarte_fmt::formatter specialization for astarte::device::Data.
template <>
struct astarte_fmt::formatter<astarte::device::Data> {
  /**
   * @brief Parses the format string.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the Data variant-like object by dispatching to the correct formatter.
   * @param[in] data The Data instance to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-function-size)
  auto format(const astarte::device::Data& data, FormatContext& ctx) const {
    auto out = ctx.out();

    if (std::holds_alternative<int32_t>(data.get_raw_data())) {
      out = astarte_fmt::format_to(out, "{}", std::get<int32_t>(data.get_raw_data()));
    } else if (std::holds_alternative<int64_t>(data.get_raw_data())) {
      out = astarte_fmt::format_to(out, "{}", std::get<int64_t>(data.get_raw_data()));
    } else if (std::holds_alternative<double>(data.get_raw_data())) {
      out = astarte_fmt::format_to(out, "{}", std::get<double>(data.get_raw_data()));
    } else if (std::holds_alternative<bool>(data.get_raw_data())) {
      const auto* bool_str = (std::get<bool>(data.get_raw_data()) ? "true" : "false");
      out = astarte_fmt::format_to(out, "{}", bool_str);
    } else if (std::holds_alternative<std::string>(data.get_raw_data())) {
      out = astarte_fmt::format_to(out, R"("{}")", std::get<std::string>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<uint8_t>>(data.get_raw_data())) {
      astarte::device::utils::format_base64(out,
                                            std::get<std::vector<uint8_t>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::chrono::system_clock::time_point>(data.get_raw_data())) {
      astarte::device::utils::format_timestamp(
          out, std::get<std::chrono::system_clock::time_point>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<int32_t>>(data.get_raw_data())) {
      astarte::device::utils::format_vector(out,
                                            std::get<std::vector<int32_t>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<int64_t>>(data.get_raw_data())) {
      astarte::device::utils::format_vector(out,
                                            std::get<std::vector<int64_t>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<double>>(data.get_raw_data())) {
      astarte::device::utils::format_vector(out,
                                            std::get<std::vector<double>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<bool>>(data.get_raw_data())) {
      astarte::device::utils::format_vector(out, std::get<std::vector<bool>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<std::string>>(data.get_raw_data())) {
      astarte::device::utils::format_vector(
          out, std::get<std::vector<std::string>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<std::vector<uint8_t>>>(data.get_raw_data())) {
      astarte::device::utils::format_vector(
          out, std::get<std::vector<std::vector<uint8_t>>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<std::chrono::system_clock::time_point>>(
                   data.get_raw_data())) {
      astarte::device::utils::format_vector(
          out, std::get<std::vector<std::chrono::system_clock::time_point>>(data.get_raw_data()));
    }

    return out;
  }
};

/**
 * @brief Stream insertion operator for Data.
 * @param[in,out] out The output stream.
 * @param[in] data The Data object to output.
 * @return A reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::Data& data) -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_DATA_H
