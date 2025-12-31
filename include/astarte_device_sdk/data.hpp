// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_DATA_H
#define ASTARTE_DEVICE_SDK_DATA_H

/**
 * @file astarte_device_sdk/data.hpp
 * @brief Astarte data class and its related methods.
 */

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "astarte_device_sdk/type.hpp"

namespace AstarteDeviceSdk {

/** @brief Restricts the allowed types for instances of an Astarte data class. */
template <typename T>
concept AstarteDataAllowedType = requires {
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

/** @brief Astarte data class, representing the basic Astarte types. */
class AstarteData {
 public:
  /**
   * @brief Constructor for the AstarteData class.
   * @note About string views. By design an AstarteData object is intended to encapsulate
   * data without relying on the lifetime of its inputs. As such passing a string_view to the
   * constructor will result in the creation of a new internal std::string object that will contain
   * a copy of the input string.
   * @param value The content of the Astarte data instance.
   */
  template <AstarteDataAllowedType T>
  explicit AstarteData(T value) {
    if constexpr (std::is_same_v<T, std::string_view>) {
      data_ = std::string(value);
    } else {
      data_ = value;
    }
  }

  /**
   * @brief Convert the Astarte data class to the appropriate data type.
   * @return The value contained in the class instance.
   */
  template <AstarteDataAllowedType T>
  [[nodiscard]] auto into() const
      -> std::conditional_t<std::is_same_v<T, std::string_view>, std::string_view, const T&> {
    if constexpr (std::is_same_v<T, std::string_view>) {
      return std::string_view(std::get<std::string>(data_));
    } else {
      return std::get<T>(data_);
    }
  }
  /**
   * @brief Convert the Astarte data class to the given type if it's the correct variant.
   * @return The value contained in the class instance or nullopt.
   */
  template <AstarteDataAllowedType T>
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
   * @brief Get the type of the data contained in this class instance.
   * @return The type of the content of this class instance.
   */
  [[nodiscard]] auto get_type() const -> AstarteType;
  /**
   * @brief Return the raw data contained in this class instance.
   * @return The raw data contained in this class instance. This is a variant containing one of the
   * possible data types.
   */
  [[nodiscard]] auto get_raw_data() const
      -> const std::variant<int32_t, int64_t, double, bool, std::string, std::vector<uint8_t>,
                            std::chrono::system_clock::time_point, std::vector<int32_t>,
                            std::vector<int64_t>, std::vector<double>, std::vector<bool>,
                            std::vector<std::string>, std::vector<std::vector<uint8_t>>,
                            std::vector<std::chrono::system_clock::time_point>>&;
  /**
   * @brief Overloader for the comparison operator ==.
   * @param other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const AstarteData& other) const -> bool;
  /**
   * @brief Overloader for the comparison operator !=.
   * @param other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const AstarteData& other) const -> bool;

 private:
  std::variant<int32_t, int64_t, double, bool, std::string, std::vector<uint8_t>,
               std::chrono::system_clock::time_point, std::vector<int32_t>, std::vector<int64_t>,
               std::vector<double>, std::vector<bool>, std::vector<std::string>,
               std::vector<std::vector<uint8_t>>,
               std::vector<std::chrono::system_clock::time_point>>
      data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_DATA_H
