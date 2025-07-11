// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_FORMATTER_H
#define ASTARTE_FORMATTER_H

#include <spdlog/fmt/fmt.h>

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#if defined(ASTARTE_FORMAT_ENABLED)
#include <libbase64.h>
#if (__cplusplus >= 202002L) && (__has_include(<format>))
#include <format>
#else  // (__cplusplus >= 202002L) && (__has_include(<format>))
#include <iomanip>
#endif  // (__cplusplus >= 202002L) && (__has_include(<format>))
#endif

#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/type.hpp"

namespace utils {

// These functions are only used for pretty printing
// NOLINTBEGIN(concurrency-mt-unsafe)
template <typename OutputIt>
void format_base64(OutputIt &out, const std::vector<uint8_t> &data) {
  const size_t input_size = data.size();
  size_t output_size = (input_size + 2) / 3 * 4;  // The +2 accounts for sizes non multiple of 3
  std::vector<char> out_str(output_size + 1);
  // Reinterpret cast is fine here as we go from `const unsigned char *` to `const char *`
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  base64_encode(reinterpret_cast<const char *>(data.data()), input_size, out_str.data(),
                &output_size, 0);

  out = fmt::format_to(out, "\"{}\"", out_str.data());
}

template <typename OutputIt>
void format_timestamp(OutputIt &out, const std::chrono::system_clock::time_point &data) {
  out = fmt::format_to(out, "\"");
#if (__cplusplus >= 202002L) && (__has_include(<format>))
  out = fmt::format_to(
      out, "{}",
      std::format("{0:%F}T{0:%T}Z", std::chrono::time_point_cast<std::chrono::milliseconds>(data)));
#else   // (__cplusplus >= 202002L) && (__has_include(<format>))
  const std::time_t time = std::chrono::system_clock::to_time_t(data);
  const std::tm utc_tm = *std::gmtime(&time);
  out = fmt::format_to(out, "{}", std::put_time(&utc_tm, "%FT%T.000Z"));
#endif  // (__cplusplus >= 202002L) && (__has_include(<format>))
  out = fmt::format_to(out, "\"");
}

template <typename OutputIt, typename T>
void format_vector(OutputIt &out, const std::vector<T> &data) {
  out = fmt::format_to(out, "[");
  for (size_t i = 0; i < data.size(); ++i) {
    out = fmt::format_to(out, "{}", data[i]);
    if (i != data.size() - 1) {
      out = fmt::format_to(out, ", ");
    }
  }
  out = fmt::format_to(out, "]");
}

template <typename OutputIt>
void format_vector_bool(OutputIt &out, const std::vector<bool> &data) {
  out = fmt::format_to(out, "[");
  for (size_t i = 0; i < data.size(); ++i) {
    out = fmt::format_to(out, "{}", (data[i] ? "true" : "false"));
    if (i != data.size() - 1) {
      out = fmt::format_to(out, ", ");
    }
  }
  out = fmt::format_to(out, "]");
}

template <typename OutputIt>
void format_vector_string(OutputIt &out, const std::vector<std::string> &data) {
  out = fmt::format_to(out, "[");
  for (size_t i = 0; i < data.size(); ++i) {
    out = fmt::format_to(out, "\"{}\"", data[i]);
    if (i != data.size() - 1) {
      out = fmt::format_to(out, ", ");
    }
  }
  out = fmt::format_to(out, "]");
}

template <typename OutputIt>
void format_vector_binaryblob(OutputIt &out, const std::vector<std::vector<uint8_t>> &data) {
  out = fmt::format_to(out, "[");
  for (size_t i = 0; i < data.size(); ++i) {
    format_base64(out, data[i]);
    if (i != data.size() - 1) {
      out = fmt::format_to(out, ", ");
    }
  }
  out = fmt::format_to(out, "]");
}

template <typename OutputIt>
void format_vector_timestamp(OutputIt &out,
                             const std::vector<std::chrono::system_clock::time_point> &data) {
  out = fmt::format_to(out, "[");
  for (size_t i = 0; i < data.size(); ++i) {
    format_timestamp(out, data[i]);
    if (i != data.size() - 1) {
      out = fmt::format_to(out, ", ");
    }
  }
  out = fmt::format_to(out, "]");
}
// NOLINTEND(concurrency-mt-unsafe)

}  // namespace utils

template <>
struct fmt::formatter<AstarteDeviceSdk::AstarteMessage> : fmt::formatter<std::string_view> {
  // The parse function is fine as it is.
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteMessage &msg, FormatContext &ctx) const {
    auto out = ctx.out();

    out = fmt::format_to(out, "{{interface: {}, path: {}", msg.get_interface(), msg.get_path());

    const std::string formatted_data =
        std::visit([](const auto &arg) { return fmt::format("{}", arg); }, msg.get_raw_data());

    if (!formatted_data.empty()) {
      out = fmt::format_to(out, ", value: {}", formatted_data);
    }

    return fmt::format_to(out, "}}");
  }
};

template <>
struct fmt::formatter<AstarteDeviceSdk::AstarteDatastreamIndividual>
    : fmt::formatter<std::string_view> {
  // The parse function is fine as it is.
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteDatastreamIndividual &data, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "{}", data.get_value());
  }
};

template <>
struct fmt::formatter<AstarteDeviceSdk::AstarteDatastreamObject>
    : fmt::formatter<std::string_view> {
  // The parse function is fine as it is.
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteDatastreamObject &data, FormatContext &ctx) const {
    auto out = ctx.out();

    bool first = true;
    for (const auto &pair : data.get_raw_data()) {
      if (!first) {
        out = fmt::format_to(out, ", ");
      }

      out = fmt::format_to(out, "\"{}\": {}", pair.first, pair.second);
      first = false;
    }

    return out;
  }
};

template <>
struct fmt::formatter<AstarteDeviceSdk::AstartePropertyIndividual>
    : fmt::formatter<std::string_view> {
  // The parse function is fine as it is.
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstartePropertyIndividual &data, FormatContext &ctx) const {
    auto out = ctx.out();

    if (data.get_value().has_value()) {
      out = fmt::format_to(out, "{}", data.get_value().value());
    }

    return out;
  }
};

template <>
struct fmt::formatter<AstarteDeviceSdk::AstarteType> : fmt::formatter<std::string_view> {
  // The parse function is fine as it is.
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteType &typ, FormatContext &ctx) const {
    std::string_view name = "Unknown Type";

    switch (typ) {
      case AstarteDeviceSdk::AstarteType::kBinaryBlob:
        name = "BinaryBlob";
        break;
      case AstarteDeviceSdk::AstarteType::kBoolean:
        name = "Boolean";
        break;
      case AstarteDeviceSdk::AstarteType::kDatetime:
        name = "Datetime";
        break;
      case AstarteDeviceSdk::AstarteType::kDouble:
        name = "Double";
        break;
      case AstarteDeviceSdk::AstarteType::kInteger:
        name = "Integer";
        break;
      case AstarteDeviceSdk::AstarteType::kLongInteger:
        name = "LongInteger";
        break;
      case AstarteDeviceSdk::AstarteType::kString:
        name = "String";
        break;
      case AstarteDeviceSdk::AstarteType::kBinaryBlobArray:
        name = "BinaryBlobArray";
        break;
      case AstarteDeviceSdk::AstarteType::kBooleanArray:
        name = "BooleanArray";
        break;
      case AstarteDeviceSdk::AstarteType::kDatetimeArray:
        name = "DatetimeArray";
        break;
      case AstarteDeviceSdk::AstarteType::kDoubleArray:
        name = "DoubleArray";
        break;
      case AstarteDeviceSdk::AstarteType::kIntegerArray:
        name = "IntegerArray";
        break;
      case AstarteDeviceSdk::AstarteType::kLongIntegerArray:
        name = "LongIntegerArray";
        break;
      case AstarteDeviceSdk::AstarteType::kStringArray:
        name = "StringArray";
        break;
    }

    return fmt::format_to(ctx.out(), "{}", name);
  }
};

template <>
struct fmt::formatter<AstarteDeviceSdk::AstarteData> : fmt::formatter<std::string_view> {
  // The parse function is fine as it is.
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteData &data, FormatContext &ctx) const {
    auto out = ctx.out();

    if (std::holds_alternative<int32_t>(data.get_raw_data())) {
      out = fmt::format_to(out, "{}", std::get<int32_t>(data.get_raw_data()));
    } else if (std::holds_alternative<int64_t>(data.get_raw_data())) {
      out = fmt::format_to(out, "{}", std::get<int64_t>(data.get_raw_data()));
    } else if (std::holds_alternative<double>(data.get_raw_data())) {
      out = fmt::format_to(out, "{}", std::get<double>(data.get_raw_data()));
    } else if (std::holds_alternative<bool>(data.get_raw_data())) {
      auto s = (std::get<bool>(data.get_raw_data()) ? "true" : "false");
      out = fmt::format_to(out, "{}", s);
    } else if (std::holds_alternative<std::string>(data.get_raw_data())) {
      out = fmt::format_to(out, "\"{}\"", std::get<std::string>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<uint8_t>>(data.get_raw_data())) {
      utils::format_base64(out, std::get<std::vector<uint8_t>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::chrono::system_clock::time_point>(data.get_raw_data())) {
      utils::format_timestamp(out,
                              std::get<std::chrono::system_clock::time_point>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<int32_t>>(data.get_raw_data())) {
      utils::format_vector(out, std::get<std::vector<int32_t>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<int64_t>>(data.get_raw_data())) {
      utils::format_vector(out, std::get<std::vector<int64_t>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<double>>(data.get_raw_data())) {
      utils::format_vector(out, std::get<std::vector<double>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<bool>>(data.get_raw_data())) {
      utils::format_vector_bool(out, std::get<std::vector<bool>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<std::string>>(data.get_raw_data())) {
      utils::format_vector_string(out, std::get<std::vector<std::string>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<std::vector<uint8_t>>>(data.get_raw_data())) {
      utils::format_vector_binaryblob(
          out, std::get<std::vector<std::vector<uint8_t>>>(data.get_raw_data()));
    } else if (std::holds_alternative<std::vector<std::chrono::system_clock::time_point>>(
                   data.get_raw_data())) {
      utils::format_vector_timestamp(
          out, std::get<std::vector<std::chrono::system_clock::time_point>>(data.get_raw_data()));
    }

    return out;
  }
};

#endif  // ASTARTE_FORMATTER_H
