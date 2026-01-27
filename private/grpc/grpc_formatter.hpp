// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GRPC_FORMATTER_H
#define GRPC_FORMATTER_H

#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>

#include <string>
#include <type_traits>

#include "astarte_device_sdk/formatter.hpp"

template <typename T>
concept ProtobufMessage = std::is_base_of_v<google::protobuf::Message, T>;

template <ProtobufMessage T>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct astarte_fmt::formatter<T> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const T& obj, FormatContext& ctx) const {
    std::string obj_str;
    google::protobuf::TextFormat::PrintToString(obj, &obj_str);
    return astarte_fmt::format_to(ctx.out(), "{}", obj_str);
  }
};

#endif  // GRPC_FORMATTER_H
