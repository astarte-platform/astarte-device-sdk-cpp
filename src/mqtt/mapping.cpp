// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/mapping.hpp"

#include <spdlog/spdlog.h>

#include <cmath>
#include <cstdint>
#include <format>
#include <limits>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/type.hpp"

namespace {
// helper function to pop the next segment off the front of the view and advances the view.
// Equivalent to finding the next '/' and moving the pointer past it.
auto pop_next_segment(std::string_view& str) -> std::string_view {
  auto pos = str.find('/');
  auto segment = str.substr(0, pos);

  if (pos == std::string_view::npos) {
    // no slash found: this is the last segment. Consume the whole string.
    str = {};
  } else {
    // slash found: Move view past the slash.
    str.remove_prefix(pos + 1);
  }
  return segment;
}

// helper function to check if the specific segment matches (handles logic for %{params})
auto is_segment_match(std::string_view pattern, std::string_view path_seg) -> bool {
  // check for parameter format to start with "%{" and end with "}"
  if (pattern.size() >= 3 && pattern.starts_with("%{") && pattern.ends_with("}")) {
    // path segment cannot be empty
    if (path_seg.empty()) {
      return false;
    }

    // path segment cannot contain forbidden chars
    return path_seg.find_first_of("#+") == std::string_view::npos;
  }

  return pattern == path_seg;
}

}  // namespace

namespace AstarteDeviceSdk {

auto Mapping::match_path(std::string_view path) const -> bool {
  // copy the endpoint
  std::string_view endpoint = endpoint_;

  // check lengths and trailing slash
  if (path.length() < 2 || path.back() == '/') {
    return false;
  }
  // check leading slash consistency
  if (endpoint.empty() || path.front() != endpoint.front()) {
    return false;
  }

  // remove the leading slash to prepare for segment iteration
  // (we know both start with same char, usually '/')
  endpoint.remove_prefix(1);
  path.remove_prefix(1);

  while (!endpoint.empty() && !path.empty()) {
    const std::string_view endpoint_seg = pop_next_segment(endpoint);
    const std::string_view path_seg = pop_next_segment(path);

    if (!is_segment_match(endpoint_seg, path_seg)) {
      return false;
    }
  }

  // both strings must be fully consumed. If one has leftovers there is a length mismatch.
  return endpoint.empty() && path.empty();
}

auto Mapping::check_data_type(const AstarteData& data) const
    -> astarte_tl::expected<void, AstarteError> {
  if (type_ != data.get_type()) {
    spdlog::error("Astarte data type and mapping type do not match");
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError("Astarte data type and mapping type do not match"));
  }

  if ((type_ == AstarteType::kDouble) && (!std::isfinite(data.into<double>()))) {
    spdlog::error("Astarte data double is not a number");
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError("Astarte data double is not a number"));
  }

  if (type_ == AstarteType::kDoubleArray) {
    for (const double value : data.into<std::vector<double>>()) {
      if (!std::isfinite(value)) {
        spdlog::error("Astarte data double is not a number");
        return astarte_tl::unexpected(
            AstarteInterfaceValidationError("Astarte data double is not a number"));
      }
    }
  }

  return {};
}

auto get_field(const json& interface, std::string_view key, json::value_t expected_type)
    -> astarte_tl::expected<json, AstarteError> {
  auto field = interface.find(key);
  if (field == interface.end()) {
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError(astarte_fmt::format("Missing required field: {}", key)));
  }

  // if we expect a signed integer, also accept an unsigned one (since the json library we use may
  // automatically convert in a wrond interger type).
  bool type_match = (field->type() == expected_type);
  if (expected_type == json::value_t::number_integer &&
      field->type() == json::value_t::number_unsigned) {
    type_match = true;
  }

  if (!type_match) {
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError(astarte_fmt::format("Field {} has invalid type", key)));
  }
  return *field;
};

}  // namespace AstarteDeviceSdk
