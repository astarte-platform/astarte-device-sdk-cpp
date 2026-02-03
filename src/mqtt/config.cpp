// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/config.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "mqtt/credentials.hpp"

namespace astarte::device::mqtt {

Config::~Config() = default;
Config::Config(Config&&) noexcept = default;
auto Config::operator=(Config&&) noexcept -> Config& = default;

Config::Config(std::string_view realm, std::string_view device_id,
               std::unique_ptr<Credential> credential, std::string_view pairing_url,
               std::string_view store_dir)
    : realm_(realm),
      device_id_(device_id),
      credential_(std::move(credential)),
      pairing_url_(pairing_url),
      store_dir_(store_dir),
      keepalive_(DEFAULT_KEEP_ALIVE),
      conn_timeout_(DEFAULT_CONNECTION_TIMEOUT),
      disconn_timeout_(DEFAULT_DISCONNECTION_TIMEOUT) {}

auto Config::with_credential_secret(std::string_view realm, std::string_view device_id,
                                    std::string_view credential, std::string_view pairing_url,
                                    std::string_view store_dir) -> Config {
  auto cred_ptr = std::make_unique<Credential>(Credential::secret(credential, store_dir));
  return {realm, device_id, std::move(cred_ptr), pairing_url, store_dir};
}

auto Config::cred_is_credential_secret() -> bool { return credential_->is_credential_secret(); }

auto Config::credential_secret() -> std::optional<std::string> {
  return credential_->is_credential_secret() ? std::optional(credential_->value()) : std::nullopt;
}

}  // namespace astarte::device::mqtt
