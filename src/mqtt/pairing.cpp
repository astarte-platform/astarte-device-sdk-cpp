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
#include "astarte_device_sdk/mqtt/crypto.hpp"

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

auto PairingApi::get_broker_url(std::string_view credential_secret, int timeout_ms) const
    -> std::string {
  auto request_url = pairing_url;
  std::string pathname =
      std::format("{}/v1/{}/devices/{}", request_url.get_pathname(), realm, device_id);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  cpr::Header auth{{"Authorization", std::format("Bearer {}", credential_secret)}};

  cpr::Response res = cpr::Get(cpr::Url{request_url.get_href()}, auth, cpr::Timeout{timeout_ms});

  if (res.error) {
    throw DeviceRegistrationException(
        std::format("Failed to retrieve Broker URL. CPR error: {}", res.error.message));
  }

  if (!is_successful(res.status_code)) {
    throw RetrieveBrokerUrlException(
        std::format("Failed to retrieve Broker URL. HTTP status code: {}, Reason: {}",
                    res.status_code, res.text));
  }

  try {
    json response_json = json::parse(res.text);
    return response_json.at("data").at("protocols").at("astarte_mqtt_v1").at("broker_url");
  } catch (const json::exception& e) {
    throw JsonAccessErrorException(
        std::format("Failed to parse JSON: {}. Body: {}", e.what(), res.text));
  }
}

auto PairingApi::get_device_cert(std::string_view credential_secret, int timeout_ms) const
    -> std::string {
  auto request_url = pairing_url;
  std::string pathname = std::format("{}/v1/{}/devices/{}/protocols/astarte_mqtt_v1/credentials",
                                     request_url.get_pathname(), realm, device_id);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  cpr::Header header{{"Content-Type", "application/json"},
                     {"Authorization", std::format("Bearer {}", credential_secret)}};

  auto device_csr = PairingApi::get_device_csr();

  json body;
  body["data"] = {{"csr", device_csr}};
  spdlog::debug("request body: {}", body.dump());

  cpr::Response res = cpr::Post(cpr::Url{request_url.get_href()}, header, cpr::Body{body.dump()},
                                cpr::Timeout{timeout_ms});

  if (res.error) {
    throw DeviceRegistrationException(std::format(
        "Failed to retrieve Astarte device certificate. CPR error: {}", res.error.message));
  }

  if (!is_successful(res.status_code)) {
    throw DeviceRegistrationException(std::format(
        "Failed to retrieve Astarte device certificate. HTTP status code: {}, Reason: {}",
        res.status_code, res.text));
  }

  try {
    json response_json = json::parse(res.text);
    return response_json.at("data").at("client_crt");
  } catch (const json::exception& e) {
    throw JsonAccessErrorException(
        std::format("Failed to parse JSON: {}. Body: {}", e.what(), res.text));
  }
}

auto PairingApi::device_cert_valid(std::string_view certificate, std::string_view credential_secret,
                                   int timeout_ms) const -> bool {
  auto request_url = pairing_url;
  std::string pathname =
      std::format("{}/v1/{}/devices/{}/protocols/astarte_mqtt_v1/credentials/verify",
                  request_url.get_pathname(), realm, device_id);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  cpr::Header header{{"Content-Type", "application/json"},
                     {"Authorization", std::format("Bearer {}", credential_secret)}};

  json body;
  body["data"] = {{"client_crt", certificate}};
  spdlog::debug("request body: {}", body.dump());

  cpr::Response res = cpr::Post(cpr::Url{request_url.get_href()}, header, cpr::Body{body.dump()},
                                cpr::Timeout{timeout_ms});

  if (res.error) {
    throw DeviceRegistrationException(std::format(
        "Failed to check Astarte device certificate validity. CPR error: {}", res.error.message));
  }

  if (!is_successful(res.status_code)) {
    throw DeviceRegistrationException(std::format(
        "Failed to check Astarte device certificate validity. HTTP status code: {}, Reason: {}",
        res.status_code, res.text));
  }

  try {
    json response_json = json::parse(res.text);
    return response_json.at("data").at("valid");
  } catch (const json::exception& e) {
    throw JsonAccessErrorException(
        std::format("Failed to parse JSON: {}. Body: {}", e.what(), res.text));
  }
}

auto PairingApi::get_device_csr() const -> std::string {
  auto priv_key = Crypto::create_key();
  return Crypto::create_csr(priv_key);
}

}  // namespace AstarteDeviceSdk
