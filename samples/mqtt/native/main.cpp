// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "config.hpp"

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::debug);

  auto cfg = Config("samples/mqtt/native/config.toml");

  try {
    auto api = AstarteDeviceSdk::ApiClient(cfg.realm, cfg.device_id, cfg.pairing_url);

    if (cfg.modes.registration_enabled()) {
      auto secret = api.register_device(cfg.pairing_jwt.value());
      spdlog::info("credential secret: {}", secret);
    }
  } catch (const std::exception& e) {
    spdlog::error("Exception thown: {}", e.what());
  }

  return 0;
}
