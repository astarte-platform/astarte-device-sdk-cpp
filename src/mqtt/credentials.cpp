// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/credentials.hpp"

#include "astarte_device_sdk/formatter.hpp"
#include "mqtt/persistence.hpp"

namespace astarte::device::mqtt {

constexpr std::string_view CLIENT_CERTIFICATE_FILE = "client-certificate.pem";
constexpr std::string_view PRIVATE_KEY_FILE = "client-priv-key.pem";

auto Credential::get_device_certificate_path(const std::string_view store_dir) -> std::string {
  return astarte_fmt::format("{}/{}", store_dir, CLIENT_CERTIFICATE_FILE);
}

auto Credential::get_device_key_path(const std::string_view store_dir) -> std::string {
  return astarte_fmt::format("{}/{}", store_dir, PRIVATE_KEY_FILE);
}

auto Credential::store_device_key_and_certificate(const std::string_view client_priv_key,
                                                  const std::string_view client_cert,
                                                  const std::string_view store_dir)
    -> astarte_tl::expected<void, Error> {
  const std::vector<std::pair<std::string, std::string_view>> files = {
      {astarte_fmt::format("{}/{}", store_dir, CLIENT_CERTIFICATE_FILE), client_cert},
      {astarte_fmt::format("{}/{}", store_dir, PRIVATE_KEY_FILE), client_priv_key}};

  for (const auto& [path, content] : files) {
    auto write_res = Persistence::write_to_file(path, content);
    if (!write_res) {
      spdlog::error("Failed to write to {}. Error: {}", path, write_res.error());
      return astarte_tl::unexpected(write_res.error());
    }
  }

  return {};
}

auto Credential::validate_client_certificate(PairingApi& api, const std::string_view secret,
                                             const std::string_view store_dir)
    -> astarte_tl::expected<bool, Error> {
  auto file_path = astarte_fmt::format("{}/{}", store_dir, CLIENT_CERTIFICATE_FILE);
  if (Persistence::exists(file_path)) {
    auto res = Persistence::read_from_file(file_path);
    if (!res) {
      // TODO(sorru94) This should be its own error
      spdlog::error("Failed to read the client certificate from {}. Error: {}", file_path,
                    res.error());
      return astarte_tl::unexpected(res.error());
    }
    auto valid = api.device_cert_valid(res.value(), secret);
    if (!valid) {
      // TODO(sorru94): This should be its own error
      spdlog::error("Failed to validate the client certificate. Error: {}", valid.error());
      return astarte_tl::unexpected(valid.error());
    }
    return valid.value();
  }
  return false;
}

auto Credential::delete_client_certificate_and_key(const std::string_view store_dir)
    -> astarte_tl::expected<void, Error> {
  return Persistence::secure_shred_file(
             astarte_fmt::format("{}/{}", store_dir, CLIENT_CERTIFICATE_FILE))
      .and_then([&]() {
        return Persistence::secure_shred_file(
            astarte_fmt::format("{}/{}", store_dir, PRIVATE_KEY_FILE));
      })
      .or_else([](const Error& err) -> astarte_tl::expected<void, Error> {
        spdlog::warn("Failed to delete client cert or private key from filesystem.");
        return astarte_tl::unexpected(err);
      });
}

}  // namespace astarte::device::mqtt
