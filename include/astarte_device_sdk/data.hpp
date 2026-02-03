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
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/type.hpp"

namespace astarte::device {

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
  explicit AstarteData(const T& value) {
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

}  // namespace astarte::device

/**
 * @brief astarte_fmt::formatter specialization for astarte::device::AstarteData.
 */
template <>
struct astarte_fmt::formatter<astarte::device::AstarteData> {
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
   * @brief Format the AstarteData variant-like object by dispatching to the correct formatter.
   * @param data The AstarteData to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-function-size)
  auto format(const astarte::device::AstarteData& data, FormatContext& ctx) const {
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
 * @brief Stream insertion operator for AstarteData.
 * @param out The output stream.
 * @param data The AstarteData object to output.
 * @return Reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::AstarteData& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_DATA_H
