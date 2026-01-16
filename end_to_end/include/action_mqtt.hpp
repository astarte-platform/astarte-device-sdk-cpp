// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"

namespace actions {

using AstarteDeviceSdk::AstarteError;
using AstarteDeviceSdk::PairingApi;

constexpr size_t CREDENTIAL_SECRET_LEN = 44;

inline Action RegisterDevice(std::string pairing_token) {
  return [token = std::move(pairing_token)](const TestCaseContext& ctx) {
    spdlog::info("Pairing device via API...");

    // Pairing requires a clean environment, typically no connected device is needed yet.
    // We use the HTTP config from the context.
    auto res =
        PairingApi::create(ctx.http.realm, ctx.device_id, ctx.http.astarte_base_url)
            .and_then([&](const PairingApi& pairing_api)
                          -> AstarteDeviceSdk::astarte_tl::expected<std::string, AstarteError> {
              return pairing_api.register_device(token);
            });

    if (!res) {
      spdlog::error("Pairing failed: {}", res.error());
      throw EndToEndAstarteDeviceException("Device pairing failed.");
    }

    if (res.value().length() != CREDENTIAL_SECRET_LEN) {
      spdlog::error("Credential secret length mismatch. Expected: {}, Actual: {}",
                    CREDENTIAL_SECRET_LEN, res.value().length());
      throw EndToEndMismatchException("Incorrect length for the credential secret.");
    }

    spdlog::info("Device paired successfully.");
  };
}

}  // namespace actions
