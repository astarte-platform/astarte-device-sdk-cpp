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

auto ApiClient::register_device(std::string_view pairing_jwt, int timeout_ms) const -> std::string {
  auto request_url = pairing_url;
  std::string pathname = std::format("{}/v1/{}/agent/devices", request_url.get_pathname(), realm);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  cpr::Header header{{"Content-Type", "application/json"}};
  cpr::Bearer auth_token{pairing_jwt};

  json body;
  body["data"] = {{"hw_id", device_id}};
  spdlog::debug("request body: {}", body.dump());

  cpr::Response res = cpr::Post(cpr::Url{request_url.get_href()}, header, auth_token,
                                cpr::Body{body.dump()}, cpr::Timeout{timeout_ms});

  if (res.error) {
    throw DeviceRegistrationException(
        std::format("Failed to register device. CPR error: {}", res.error.message));
  }

  if (res.status_code != 201) {
    throw DeviceRegistrationException(
        std::format("Failed to register device. HTTP status code: {}", res.status_code));
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
