// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GRPC_FORMATTER_H
#define GRPC_FORMATTER_H

#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>

#include <string>

#include "astarte_device_sdk/formatter.hpp"

#if __cplusplus >= 202002L
#include <type_traits>

template <typename T>
concept ProtobufMessage = std::is_base_of_v<google::protobuf::Message, T>;

template <ProtobufMessage T>
struct NS_FORMAT::formatter<T> {  // NOLINT
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const T &obj, FormatContext &ctx) const {
    std::string obj_str;
    google::protobuf::TextFormat::PrintToString(obj, &obj_str);
    return NS_FORMAT::format_to(ctx.out(), "{}", obj_str);
  }
};
#else   // __cplusplus >= 202002L
template <typename T>
using enable_if_protobuf = std::enable_if_t<std::is_base_of_v<google::protobuf::Message, T> >;

template <typename T>
struct NS_FORMAT::formatter<T, char, enable_if_protobuf<T> > {  // NOLINT
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const T &obj, FormatContext &ctx) const {
    std::string obj_str;
    google::protobuf::TextFormat::PrintToString(obj, &obj_str);
    return NS_FORMAT::format_to(ctx.out(), "{}", obj_str);
  }
};
#endif  // __cplusplus >= 202002L

#endif  // GRPC_FORMATTER_H
