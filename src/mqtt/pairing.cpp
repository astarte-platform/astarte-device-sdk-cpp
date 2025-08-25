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

  try {
    json response_json = json::parse(res.text);

    if (res.status_code != 201) {
      std::array reasons = response_json.at("errors").at("error_name");

      std::string reason;
      reason += "[";
      for (size_t i = 0; i < reasons.size(); ++i) {
        reason += reasons[i];
        if (i != reasons.size() - 1) {
          reason += ", ";
        }
      }
      reason += "]";

      throw DeviceRegistrationException(std::format(
          "Failed to register device. HTTP status code: {}. Reason: {}", res.status_code, reason));
    }

    return response_json.at("data").at("credentials_secret");
  } catch (const json::exception& e) {
    throw JsonAccessErrorException(
        std::format("Failed to parse JSON: {}. Body: {}", e.what(), res.text));
  }
}

auto ApiClient::get_broker_url(std::string_view credential_secret, int timeout_ms) const
    -> std::string {
  auto request_url = pairing_url;
  std::string pathname =
      std::format("{}/v1/{}/devices/{}", request_url.get_pathname(), realm, device_id);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  cpr::Bearer auth{credential_secret};

  cpr::Response res = cpr::Get(cpr::Url{request_url.get_href()}, auth, cpr::Timeout{timeout_ms});

  if (res.error) {
    throw DeviceRegistrationException(
        std::format("Failed to retrieve Broker URL. CPR error: {}", res.error.message));
  }

  if (res.status_code != 200) {
    throw RetrieveBrokerUrlException(
        std::format("Failed to retrieve Broker URL. HTTP status code: {}", res.status_code));
  }

  try {
    json response_json = json::parse(res.text);
    return response_json.at("data").at("protocols").at("astarte_mqtt_v1").at("broker_url");
  } catch (const json::exception& e) {
    throw JsonAccessErrorException(
        std::format("Failed to parse JSON: {}. Body: {}", e.what(), res.text));
  }
}

auto ApiClient::get_device_cert(std::string_view credential_secret, int timeout_ms) const
    -> std::string {
  auto request_url = pairing_url;
  std::string pathname = std::format("{}/v1/{}/devices/{}/protocols/astarte_mqtt_v1/credentials",
                                     request_url.get_pathname(), realm, device_id);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  cpr::Header header{{"Content-Type", "application/json"}};
  cpr::Bearer auth_token{credential_secret};

  auto device_csr = ApiClient::get_device_csr();

  json body;
  body["data"] = {{"csr", device_csr}};
  spdlog::debug("request body: {}", body.dump());

  cpr::Response res = cpr::Post(cpr::Url{request_url.get_href()}, header, auth_token,
                                cpr::Body{body.dump()}, cpr::Timeout{timeout_ms});

  if (res.error) {
    throw DeviceRegistrationException(std::format(
        "Failed to retrieve Astarte device certificate. CPR error: {}", res.error.message));
  }

  if (res.status_code != 201) {
    throw DeviceRegistrationException(std::format(
        "Failed to retrieve Astarte device certificate. HTTP status code: {}", res.status_code));
  }

  try {
    json response_json = json::parse(res.text);
    return response_json.at("data").at("client_crt");
  } catch (const json::exception& e) {
    throw JsonAccessErrorException(
        std::format("Failed to parse JSON: {}. Body: {}", e.what(), res.text));
  }
}

auto ApiClient::device_cert_valid(std::string_view certificate, std::string_view credential_secret,
                                  int timeout_ms) const -> bool {
  auto request_url = pairing_url;
  std::string pathname =
      std::format("{}/v1/{}/devices/{}/protocols/astarte_mqtt_v1/credentials/verify",
                  request_url.get_pathname(), realm, device_id);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  cpr::Header header{{"Content-Type", "application/json"}};
  cpr::Bearer auth_token{credential_secret};

  json body;
  body["data"] = {{"client_crt", certificate}};
  spdlog::debug("request body: {}", body.dump());

  cpr::Response res = cpr::Post(cpr::Url{request_url.get_href()}, header, auth_token,
                                cpr::Body{body.dump()}, cpr::Timeout{timeout_ms});

  if (res.error) {
    throw DeviceRegistrationException(std::format(
        "Failed to check Astarte device certificate validity. CPR error: {}", res.error.message));
  }

  if (res.status_code != 200) {
    throw DeviceRegistrationException(
        std::format("Failed to check Astarte device certificate validity. HTTP status code: {}",
                    res.status_code));
  }

  try {
    json response_json = json::parse(res.text);
    // TODO: save also the returned "until" value (define a struct to store both "valid" and "until"
    // values)
    return response_json.at("data").at("valid");
  } catch (const json::exception& e) {
    throw JsonAccessErrorException(
        std::format("Failed to parse JSON: {}. Body: {}", e.what(), res.text));
  }
}

auto ApiClient::get_device_csr() const -> std::string {
  auto priv_key = Crypto::create_key();
  // TODO: store the private key in an option inside the ApiClient struct
  return Crypto::create_csr(priv_key);
}

}  // namespace AstarteDeviceSdk
