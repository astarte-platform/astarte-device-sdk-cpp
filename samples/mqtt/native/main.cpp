// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "config.hpp"

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::debug);

  auto cfg = Config("samples/mqtt/native/config.toml");

  if (cfg.modes.registration_enabled()) {
    auto secret = AstarteDeviceSdk::register_device(cfg.pairing_jwt.value(), cfg.pairing_url,
                                                    cfg.realm, cfg.device_id);
    if (secret.has_value()) {
      spdlog::info("credential secret: {}", secret.value());
    }
  }

  return 0;
}
