// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include "astarte_device_sdk/mqtt/crypto.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "config.hpp"

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::debug);

  auto cfg = Config("samples/mqtt/native/config.toml");

  try {
    auto device_id = AstarteDeviceSdk::Crypto::create_random_device_id();
    spdlog::info("random device id: {}", device_id);

    auto api = AstarteDeviceSdk::PairingApi(cfg.realm, device_id, cfg.pairing_url);

    if (cfg.features.registration_enabled()) {
      auto secret = api.register_device(cfg.pairing_jwt.value());
      spdlog::info("credential secret: {}", secret);

      auto broker_url = api.get_broker_url(secret);
      spdlog::info("broker url: {}", broker_url);
    }
  } catch (const std::exception& e) {
    spdlog::error("Exception thown: {}", e.what());
  }

  return 0;
}
