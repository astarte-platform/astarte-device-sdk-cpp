// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_MSG_H
#define ASTARTE_DEVICE_SDK_MSG_H

/**
 * @file astarte_device_sdk/msg.hpp
 * @brief Astarte message class and its related methods.
 *
 * @details This file defines the message container used to represent data packets
 * exchanged with the Astarte platform, encapsulating interface names, paths, and payloads.
 */

#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/property.hpp"

namespace astarte::device {

/**
 * @brief Astarte message class, represents a full message for/from Astarte.
 *
 * @details The `Message` class acts as a unified container for data received from
 * Astarte (via polling). It can hold individual datastreams, aggregated objects, or
 * individual properties.
 */
class Message {
 public:
  /**
   * @brief Constructor for the Message class.
   *
   * @tparam T The type of the data payload.
   * @param[in] interface The interface for the message.
   * @param[in] path The path for the message.
   * @param[in] data The data for the message.
   */
  template <typename T>
  Message(std::string_view interface, std::string_view path, T data)
      : interface_(interface), path_(path), data_(std::move(data)) {}

  /**
   * @brief Gets the interface of the message.
   *
   * @return A constant reference to the interface name.
   */
  [[nodiscard]] auto get_interface() const -> const std::string&;

  /**
   * @brief Gets the path of the message.
   *
   * @return A constant reference to the path.
   */
  [[nodiscard]] auto get_path() const -> const std::string&;

  /**
   * @brief Checks if this message contains a datastream.
   *
   * @return True if the message contains a datastream, false otherwise.
   */
  [[nodiscard]] auto is_datastream() const -> bool;

  /**
   * @brief Checks if this message contains individual data.
   *
   * @return True if the message contains individual data, false otherwise.
   */
  [[nodiscard]] auto is_individual() const -> bool;

  /**
   * @brief Get the content of the message.
   *
   * @tparam T The target type to cast the content to.
   * @return A constant reference to the value contained in the message.
   */
  template <typename T>
  [[nodiscard]] auto into() const -> const T& {
    return std::get<T>(data_);
  }

  /**
   * @brief Return the content of the message if it's of the correct type.
   *
   * @tparam T The target type to attempt casting to.
   * @return std::optional containing the value if the type matches, or std::nullopt.
   */
  template <typename T>
  [[nodiscard]] auto try_into() const -> std::optional<T> {
    if (std::holds_alternative<T>(data_)) {
      return std::get<T>(data_);
    }

    return std::nullopt;
  }

  /**
   * @brief Returns the raw data contained in this class instance.
   *
   * @return A constant reference to the internal variant containing the data.
   */
  [[nodiscard]] auto get_raw_data() const
      -> const std::variant<DatastreamIndividual, DatastreamObject, PropertyIndividual>&;

  /**
   * @brief Overloader for the comparison operator ==.
   *
   * @param[in] other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const Message& other) const -> bool;

  /**
   * @brief Overloader for the comparison operator !=.
   *
   * @param[in] other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const Message& other) const -> bool;

 private:
  std::string interface_;
  std::string path_;
  std::variant<DatastreamIndividual, DatastreamObject, PropertyIndividual> data_;
};

}  // namespace astarte::device

/// @brief astarte_fmt::formatter specialization for astarte::device::Message.
template <>
struct astarte_fmt::formatter<astarte::device::Message> {
  /**
   * @brief Parses the format string.
   *
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the Message object.
   *
   * @param[in] msg The Message instance to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::Message& msg, FormatContext& ctx) const {
    auto out = ctx.out();

    out = astarte_fmt::format_to(out, "{{interface: {}, path: {}", msg.get_interface(),
                                 msg.get_path());

    // check if the payload is an unset property, which is the only "empty" case
    bool is_unset_prop = false;
    const auto* prop = std::get_if<astarte::device::PropertyIndividual>(&msg.get_raw_data());
    if (prop && !prop->get_value().has_value()) {
      is_unset_prop = true;
    }

    if (!is_unset_prop) {
      out = astarte_fmt::format_to(out, ", value: ");
      std::visit([&out](const auto& arg) { out = astarte_fmt::format_to(out, "{}", arg); },
                 msg.get_raw_data());
    }

    return astarte_fmt::format_to(out, "}}");
  }
};

/**
 * @brief Stream insertion operator for Message.
 *
 * @param[in,out] out The output stream.
 * @param[in] msg The Message object to output.
 * @return A reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::Message& msg) -> std::ostream& {
  out << astarte_fmt::format("{}", msg);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_MSG_H
