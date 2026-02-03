// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_MSG_H
#define ASTARTE_DEVICE_SDK_MSG_H

/**
 * @file astarte_device_sdk/msg.hpp
 * @brief Astarte message class and its related methods.
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

/** @brief Astarte message class, represents a full message for/from Astarte. */
class AstarteMessage {
 public:
  /**
   * @brief Constructor for the AstarteMessage class.
   * @param interface The interface for the message.
   * @param path The path for the message.
   * @param data The data for the message.
   */
  template <typename T>
  AstarteMessage(std::string_view interface, std::string_view path, T data)
      : interface_(interface), path_(path), data_(std::move(data)) {}

  /**
   * @brief Get the interface of the message.
   * @return The interface.
   */
  [[nodiscard]] auto get_interface() const -> const std::string&;
  /**
   * @brief Get the path of the message.
   * @return The path.
   */
  [[nodiscard]] auto get_path() const -> const std::string&;
  /**
   * @brief Check if this message contains a datastream.
   * @return True if the message contains a datastream, false otherwise.
   */
  [[nodiscard]] auto is_datastream() const -> bool;
  /**
   * @brief Check if this message contains individual data.
   * @return True if the message contains individual data, false otherwise.
   */
  [[nodiscard]] auto is_individual() const -> bool;
  /**
   * @brief Get the content of the message.
   * @return The value contained in the message.
   */
  template <typename T>
  [[nodiscard]] auto into() const -> const T& {
    return std::get<T>(data_);
  }
  /**
   * @brief Return the content of the message if it's of the correct type.
   * @return The value contained in the message or nullopt.
   */
  template <typename T>
  [[nodiscard]] auto try_into() const -> std::optional<T> {
    if (std::holds_alternative<T>(data_)) {
      return std::get<T>(data_);
    }

    return std::nullopt;
  }
  /**
   * @brief Return the raw data contained in this class instance.
   * @return The raw data contained in this class instance.
   */
  [[nodiscard]] auto get_raw_data() const -> const
      std::variant<DatastreamIndividual, AstarteDatastreamObject, AstartePropertyIndividual>&;
  /**
   * @brief Overloader for the comparison operator ==.
   * @param other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const AstarteMessage& other) const -> bool;
  /**
   * @brief Overloader for the comparison operator !=.
   * @param other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const AstarteMessage& other) const -> bool;

 private:
  std::string interface_;
  std::string path_;
  std::variant<DatastreamIndividual, AstarteDatastreamObject, AstartePropertyIndividual> data_;
};

}  // namespace astarte::device

/**
 * @brief astarte_fmt::formatter specialization for astarte::device::AstarteMessage.
 */
template <>
struct astarte_fmt::formatter<astarte::device::AstarteMessage> {
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
   * @brief Format the AstarteMessage object.
   * @param msg The AstarteMessage to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::AstarteMessage& msg, FormatContext& ctx) const {
    auto out = ctx.out();

    out = astarte_fmt::format_to(out, "{{interface: {}, path: {}", msg.get_interface(),
                                 msg.get_path());

    // check if the payload is an unset property, which is the only "empty" case
    bool is_unset_prop = false;
    const auto* prop = std::get_if<astarte::device::AstartePropertyIndividual>(&msg.get_raw_data());
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
 * @brief Stream insertion operator for AstarteMessage.
 * @param out The output stream.
 * @param msg The AstarteMessage object to output.
 * @return Reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::AstarteMessage& msg)
    -> std::ostream& {
  out << astarte_fmt::format("{}", msg);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_MSG_H
