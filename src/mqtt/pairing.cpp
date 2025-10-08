// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/pairing.hpp"

#include <cpr/cpr.h>
#include <spdlog/spdlog.h>

#include <format>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

#include "ada.h"

using json = nlohmann::json;

namespace AstarteDeviceSdk {

auto PairingApi::parse_and_validate_url(std::string_view url) -> ada::url_aggregator {
  auto parsed_url = ada::parse(url);
  if (!parsed_url) {
    throw InvalidUrlException(
        std::format("Failed to register device. Provided wrong pairing URL: {}", url));
  }
  return std::move(parsed_url.value());
}

// check if the response code of an HTTP is successfull (i.e., 2XX) or not
auto is_successful(long status_code) -> bool { return (status_code / 100) == 2; }

auto PairingApi::register_device(std::string_view pairing_token, int timeout_ms) const
    -> std::string {
  auto request_url = pairing_url;
  std::string pathname = std::format("{}/v1/{}/agent/devices", request_url.get_pathname(), realm);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  cpr::Header header{{"Content-Type", "application/json"},
                     {"Authorization", std::format("Bearer {}", pairing_token)}};

  json body;
  body["data"] = {{"hw_id", device_id}};
  spdlog::debug("request body: {}", body.dump());

  cpr::Response res = cpr::Post(cpr::Url{request_url.get_href()}, header, cpr::Body{body.dump()},
                                cpr::Timeout{timeout_ms});

  // it tells whether a generic error occurred (e.g., a timeout, the device was already registeres
  // and its certificate expired, etc.)
  if (res.error) {
    throw DeviceRegistrationException(
        std::format("Failed to register device. CPR error: {}", res.error.message));
  }

  if (!is_successful(res.status_code)) {
    throw DeviceRegistrationException(std::format(
        "Failed to register device. HTTP status code: {}, Reason: {}", res.status_code, res.text));
  }

  try {
    json response_json = json::parse(res.text);
    return response_json.at("data").at("credentials_secret");
  } catch (const json::exception& e) {
    throw JsonAccessErrorException(
        std::format("Failed to parse JSON: {}. Body: {}", e.what(), res.text));
  }
}

}  // namespace AstarteDeviceSdk
