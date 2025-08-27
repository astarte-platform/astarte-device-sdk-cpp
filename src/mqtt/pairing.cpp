// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

// TODO(sorru94) Consider disabling this checker fully or disable the MissingInclude setting
// NOLINTBEGIN(misc-include-cleaner) PSA structures are not intended to be included directly

#include "astarte_device_sdk/mqtt/pairing.hpp"

#include <ada.h>
#include <cpr/cpr.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <format>
#include <nlohmann/json.hpp>
#include <random>
#include <string>
#include <string_view>
#include <utility>

#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "mqtt/crypto.hpp"
#include "uuid.h"

using json = nlohmann::json;

namespace AstarteDeviceSdk {

/**
 * @brief Format a vector of bytes into a Base64 URL safe string literal.
 * @param data The vector of bytes to format.
 * @return Base64 URL safe encoded string
 */
auto format_base64_url_safe(const std::vector<uint8_t>& data) -> std::string {
  static constexpr std::string_view base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789-_";

  std::string out;
  out.reserve(((data.size() + 2) / 3) * 4);  // pre-allocate memory for efficiency

  size_t idx = 0;
  const size_t len = data.size();

  // process full 3-byte chunks
  while (idx + 2 < len) {
    const uint32_t chunk = (static_cast<uint32_t>(data[idx]) << 16) |
                           (static_cast<uint32_t>(data[idx + 1]) << 8) | data[idx + 2];
    out += base64_chars[(chunk >> 18) & 0x3F];
    out += base64_chars[(chunk >> 12) & 0x3F];
    out += base64_chars[(chunk >> 6) & 0x3F];
    out += base64_chars[chunk & 0x3F];
    idx += 3;
  }

  // handle remaining bytes
  if (idx < len) {
    uint32_t chunk = static_cast<uint32_t>(data[idx]) << 16;

    out += base64_chars[(chunk >> 18) & 0x3F];
    out += base64_chars[(chunk >> 12) & 0x3F];

    if (idx + 1 < len) {  // two bytes left
      chunk |= static_cast<uint32_t>(data[idx + 1]) << 8;
      out += base64_chars[(chunk >> 6) & 0x3F];
    }
  }

  return out;
}

auto bytes_to_uuid_str(const std::span<std::byte const, 16> bytes) -> std::string {
  std::vector<uint8_t> bytes_v(reinterpret_cast<const uint8_t*>(bytes.data()),
                               reinterpret_cast<const uint8_t*>(bytes.data()) + bytes.size());

  return format_base64_url_safe(bytes_v);
}

namespace {

constexpr std::size_t htpp_category_divisor = 100;
enum class HttpStatusCategory : std::uint8_t {
  Informational = 1,  // 1xx
  Success = 2,        // 2xx
  Redirection = 3,    // 3xx
  ClientError = 4,    // 4xx
  ServerError = 5     // 5xx
};

auto is_successful(std::int64_t status_code) -> bool {
  return static_cast<HttpStatusCategory>(status_code / htpp_category_divisor) ==
         HttpStatusCategory::Success;
}

template <typename T>
struct json_type_traits;
template <>
struct json_type_traits<std::string> {
  static auto check(const json& input) -> bool { return input.is_string(); }
  static auto name() -> const char* { return "string"; }
};
template <>
struct json_type_traits<bool> {
  static auto check(const json& input) -> bool { return input.is_boolean(); }
  static auto name() -> const char* { return "boolean"; }
};

template <typename T>
auto parse_json(const std::string& text, const std::string& path)
    -> astarte_tl::expected<T, AstarteError> {
  json text_json = json::parse(text, nullptr, false);
  if (text_json.is_discarded()) {
    return astarte_tl::unexpected(
        AstarteJsonParsingError(astarte_fmt::format("Invalid JSON. Body: {}", text)));
  }

  const json::json_pointer path_json(path);
  if (!text_json.contains(path_json)) {
    return astarte_tl::unexpected(AstarteJsonParsingError{
        astarte_fmt::format("Path {} not found. Body: {}", path_json.to_string(), text)});
  }

  const auto& value = text_json[path_json];
  if (!json_type_traits<T>::check(value)) {
    return astarte_tl::unexpected(AstarteJsonParsingError{
        astarte_fmt::format("Value at {} is not a {}. Body: {}", path_json.to_string(),
                            json_type_traits<T>::name(), text)});
  }
  return value;
}

}  // namespace

auto PairingApi::create(std::string_view realm, std::string_view device_id,
                        std::string_view astarte_base_url)
    -> astarte_tl::expected<PairingApi, AstarteError> {
  auto parsed_url = ada::parse(astarte_base_url);
  if (!parsed_url) {
    return astarte_tl::unexpected(AstarteMqttError(AstartePairingApiError(
        "Failed creating the pairing API class",
        AstarteInvalidUrlError(astarte_fmt::format("Invalid base URL: {}", astarte_base_url)))));
  }

  auto pairing_url = parsed_url.value();
  pairing_url.set_pathname("pairing");
  return PairingApi(realm, device_id, pairing_url);
}

PairingApi::PairingApi(std::string_view realm, std::string_view device_id,
                       ada::url_aggregator pairing_url)
    : realm_(realm), device_id_(device_id), pairing_url_(std::move(pairing_url)) {}

auto PairingApi::register_device(std::string_view pairing_token,
                                 std::chrono::milliseconds timeout_ms) const
    -> astarte_tl::expected<std::string, AstarteError> {
  auto request_url = pairing_url_;
  const std::string pathname =
      astarte_fmt::format("{}/v1/{}/agent/devices", request_url.get_pathname(), realm_);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  const cpr::Header header{{"Content-Type", "application/json"},
                           {"Authorization", astarte_fmt::format("Bearer {}", pairing_token)}};

  json body;
  body["data"] = {{"hw_id", device_id_}};
  spdlog::debug("request body: {}", body.dump());

  cpr::Response res = cpr::Post(cpr::Url{request_url.get_href()}, header, cpr::Body{body.dump()},
                                cpr::Timeout{timeout_ms});

  // it tells whether a generic error occurred (e.g., a timeout, the device was already registeres
  // and its certificate expired, etc.)
  if (res.error) {
    return astarte_tl::unexpected(AstarteMqttError(AstartePairingApiError(
        "Failed to register device.",
        AstarteHttpError(astarte_fmt::format("CPR error: {}", res.error.message)))));
  }

  if (!is_successful(res.status_code)) {
    return astarte_tl::unexpected(AstarteMqttError(
        AstartePairingApiError("Failed to register device.",
                               AstarteHttpError(astarte_fmt::format("Sttatus code: {}, Reason: {}",
                                                                    res.status_code, res.text)))));
  }

  return parse_json<std::string>(res.text, "/data/credentials_secret");
}

auto PairingApi::get_broker_url(std::string_view credential_secret, int timeout_ms) const
    -> astarte_tl::expected<std::string, AstarteError> {
  auto request_url = pairing_url_;
  const std::string pathname =
      astarte_fmt::format("{}/v1/{}/devices/{}", request_url.get_pathname(), realm_, device_id_);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  const cpr::Header auth{{"Authorization", astarte_fmt::format("Bearer {}", credential_secret)}};

  cpr::Response res = cpr::Get(cpr::Url{request_url.get_href()}, auth, cpr::Timeout{timeout_ms});

  if (res.error) {
    return astarte_tl::unexpected(AstarteMqttError(AstartePairingApiError(
        "Failed to retrieve Broker URL.",
        AstarteHttpError(astarte_fmt::format("CPR error: {}", res.error.message)))));
  }

  if (!is_successful(res.status_code)) {
    return astarte_tl::unexpected(AstarteMqttError(
        AstartePairingApiError("Failed to retrieve Broker URL.",
                               AstarteHttpError(astarte_fmt::format("Status code: {}, Reason: {}",
                                                                    res.status_code, res.text)))));
  }

  return parse_json<std::string>(res.text, "/data/protocols/astarte_mqtt_v1/broker_url");
}

auto PairingApi::get_device_cert(std::string_view credential_secret, int timeout_ms) const
    -> astarte_tl::expected<std::string, AstarteError> {
  auto request_url = pairing_url_;
  const std::string pathname =
      astarte_fmt::format("{}/v1/{}/devices/{}/protocols/astarte_mqtt_v1/credentials",
                          request_url.get_pathname(), realm_, device_id_);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  const cpr::Header header{{"Content-Type", "application/json"},
                           {"Authorization", astarte_fmt::format("Bearer {}", credential_secret)}};

  auto priv_key_res = PsaKey::create();
  if (!priv_key_res) {
    return astarte_tl::unexpected(priv_key_res.error());
  }
  auto& priv_key = priv_key_res.value();
  auto priv_key_generate_res = priv_key.generate();
  if (!priv_key_generate_res) {
    return astarte_tl::unexpected(priv_key_generate_res.error());
  }
  auto device_csr = Crypto::create_csr(priv_key);
  if (!device_csr) {
    return astarte_tl::unexpected(AstarteMqttError(AstartePairingApiError(
        "Failed to retrieve Astarte device certificate.", device_csr.error())));
  }

  json body;
  body["data"] = {{"csr", device_csr.value()}};
  spdlog::debug("request body: {}", body.dump());

  cpr::Response res = cpr::Post(cpr::Url{request_url.get_href()}, header, cpr::Body{body.dump()},
                                cpr::Timeout{timeout_ms});

  if (res.error) {
    return astarte_tl::unexpected(AstarteMqttError(AstartePairingApiError(
        "Failed to retrieve Astarte device certificate.",
        AstarteHttpError(astarte_fmt::format("CPR error: {}", res.error.message)))));
  }

  if (!is_successful(res.status_code)) {
    return astarte_tl::unexpected(AstarteMqttError(
        AstartePairingApiError("Failed to retrieve Astarte device certificate.",
                               AstarteHttpError(astarte_fmt::format("Status code: {}, Reason: {}",
                                                                    res.status_code, res.text)))));
  }

  return parse_json<std::string>(res.text, "/data/client_crt")
      .transform_error([](const AstarteError& err) -> AstarteError {
        return AstarteMqttError(
            AstartePairingApiError("Failed to retrieve Astarte device certificate.", err));
      });
}

auto PairingApi::device_cert_valid(std::string_view certificate, std::string_view credential_secret,
                                   int timeout_ms) const
    -> astarte_tl::expected<bool, AstarteError> {
  auto request_url = pairing_url_;
  const std::string pathname =
      astarte_fmt::format("{}/v1/{}/devices/{}/protocols/astarte_mqtt_v1/credentials/verify",
                          request_url.get_pathname(), realm_, device_id_);
  request_url.set_pathname(pathname);
  spdlog::debug("request url: {}", request_url.get_href());

  const cpr::Header header{{"Content-Type", "application/json"},
                           {"Authorization", astarte_fmt::format("Bearer {}", credential_secret)}};

  json body;
  body["data"] = {{"client_crt", certificate}};
  spdlog::debug("request body: {}", body.dump());

  cpr::Response res = cpr::Post(cpr::Url{request_url.get_href()}, header, cpr::Body{body.dump()},
                                cpr::Timeout{timeout_ms});

  if (res.error) {
    return astarte_tl::unexpected(AstarteMqttError(AstartePairingApiError(
        "Failed to check Astarte device certificate validity.",
        AstarteHttpError(astarte_fmt::format("CPR error: {}", res.error.message)))));
  }

  if (!is_successful(res.status_code)) {
    return astarte_tl::unexpected(AstarteMqttError(
        AstartePairingApiError("Failed to check Astarte device certificate validity.",
                               AstarteHttpError(astarte_fmt::format("Status code: {}, Reason: {}",
                                                                    res.status_code, res.text)))));
  }

  return parse_json<bool>(res.text, "/data/valid")
      .transform_error([](const AstarteError& err) -> AstarteError {
        return AstarteMqttError(
            AstartePairingApiError("Failed to check Astarte device certificate validity.", err));
      });
}

auto create_random_device_id() -> std::string {
  // create a seed for the uuid generator
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 engine(seq);

  // pass the engine to the UUID generator's constructor
  uuids::uuid_random_generator gen(engine);

  // generate a v4 UUID
  uuids::uuid const uuid = gen();
  return bytes_to_uuid_str(uuid.as_bytes());
}

auto create_deterministic_device_id(std::string_view namespc, std::string_view unique_data)
    -> astarte_tl::expected<std::string, AstarteError> {
  // generate a v5 (name-based, SHA-1) UUID starting from the string namespace UUID representation
  auto ns = uuids::uuid::from_string(namespc);

  if (!ns.has_value()) {
    return astarte_tl::unexpected(AstarteUuidError(
        astarte_fmt::format("Couldn't parse namespace to UUID, invalid value: {}", namespc)));
  }

  // Create a name generator seeded with the namespace
  uuids::uuid_name_generator v5_generator(ns.value());
  uuids::uuid const uuid = v5_generator(unique_data);

  return bytes_to_uuid_str(uuid.as_bytes());
}

}  // namespace AstarteDeviceSdk

// NOLINTEND(misc-include-cleaner)
