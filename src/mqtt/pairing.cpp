// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/pairing.hpp"

#include <cpr/cpr.h>
#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include "ada.h"

using json = nlohmann::json;

namespace AstarteDeviceSdk {

auto register_device(std::string_view pairing_jwt, std::string_view pairing_url,
                     std::string_view realm, std::string_view device_id, int timeout_ms)
    -> std::optional<std::string> {
  // perform url validity check
  auto url = ada::parse(pairing_url);
  if (!url) {
    spdlog::error("Failed to register device. Provided wrong pairing URL: {}", pairing_url);
    return std::nullopt;
  }
  std::stringstream pathname;
  pathname << url->get_pathname() << "/v1/" << realm << "/agent/devices";
  url->set_pathname(pathname.str());

  cpr::Header header{{"Content-Type", "application/json"}};
  cpr::Bearer auth_token{pairing_jwt};

  json body;
  body["data"] = {{"hw_id", device_id}};

  cpr::Response res = cpr::Post(cpr::Url{url->get_href()}, header, auth_token,
                                cpr::Body{body.dump()}, cpr::Timeout{timeout_ms});

  // check for transport-level errors first (like timeouts, connection refused, etc.)
  if (res.error) {
    spdlog::error("Failed to register device. CPR error: {}", res.error.message);
    return std::nullopt;
  }

  if (res.status_code != 201) {
    spdlog::error("Failed to register device. HTTP status code: {}. Body: {}", res.status_code,
                  res.text);
    return std::nullopt;
  }

  return json::parse(res.text)["data"]["credentials_secret"];
}

}  // namespace AstarteDeviceSdk
