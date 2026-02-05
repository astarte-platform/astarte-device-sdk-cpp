// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GRPC_FORMATTER_H
#define GRPC_FORMATTER_H

/**
 * @file private/grpc/grpc_formatter.hpp
 * @brief Formatting support for Protobuf messages in the Astarte SDK.
 *
 * @details This file defines a formatter specialization that allows Google Protobuf
 * messages to be formatted directly using the astarte_fmt library (wraps fmt or std::format).
 * It uses the Protobuf `TextFormat` printer to produce human-readable strings.
 */

#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>

#include <string>
#include <type_traits>

#include "astarte_device_sdk/formatter.hpp"

/// @brief Concept to identify types derived from google::protobuf::Message.
template <typename T>
concept ProtobufMessage = std::is_base_of_v<google::protobuf::Message, T>;

/**
 * @brief Formatter specialization for Protobuf messages.
 *
 * @details Enables formatting of any class derived from `google::protobuf::Message`
 * by using `google::protobuf::TextFormat::PrintToString` to generate a string representation.
 *
 * @tparam T The specific Protobuf message type.
 */
template <ProtobufMessage T>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct astarte_fmt::formatter<T> {
  /**
   * @brief Parses the format string.
   * @details Standard parse implementation.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  /**
   * @brief Formats the Protobuf message.
   * @details Converts the message to a string using Protobuf's text format and writes it to the
   * context output.
   *
   * @param[in] obj The Protobuf message to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const T& obj, FormatContext& ctx) const {
    std::string obj_str;
    google::protobuf::TextFormat::PrintToString(obj, &obj_str);
    return astarte_fmt::format_to(ctx.out(), "{}", obj_str);
  }
};

#endif  // GRPC_FORMATTER_H
