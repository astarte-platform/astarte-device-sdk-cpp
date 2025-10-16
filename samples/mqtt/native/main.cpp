// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include <format>
#include <optional>
#include <string>
#include <string_view>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "config.hpp"
#include "store.hpp"

int main() {
  spdlog::set_level(spdlog::level::trace);

  auto cfg = Config("samples/mqtt/native/config.toml");

  try {
    auto api = AstarteDeviceSdk::PairingApi(cfg.realm, cfg.device_id, cfg.astarte_base_url);
    auto db = init_db("samples/mqtt/native/example.db");

    // flag used to state if the credential secret has been stored in db
    bool in_db = false;
    // check if the device is already registered
    auto cred_opt = credential_secret_from_db(db, cfg.device_id);
    if (cred_opt) {
      spdlog::debug("device credential secret found in db");
      in_db = true;
    }

    if (cfg.features.registration_enabled() && !in_db) {
      assert(cfg.pairing_token);

      auto secret = api.register_device(cfg.pairing_token.value());
      spdlog::info("credential secret: {}", secret);
        auto [key, cert] = api.get_device_key_and_cert(secret);
        spdlog::info("key: {}", key);
        spdlog::info("cert: {}", cert);

      store_cred_secret(db, cfg.device_id, secret);
      in_db = true;
      cred_opt = std::optional(secret);
    }

    if (cfg.features.connection_enabled()) {
      if (!in_db && !cfg.pairing_token && !cfg.credential_secret) {
        spdlog::error("neither pairing token nor credential secret has been set");
        return 1;
      }

      const auto mqtt_cfg = [&] {
        if (in_db) {
          return AstarteDeviceSdk::MqttConfig::with_credential_secret(
              cfg.realm, cfg.device_id, *cred_opt, std::format("{}/pairing", cfg.astarte_base_url),
              cfg.store_dir);
        } else if (cfg.credential_secret) {
          // first, store the cred secret in the db for future usage
          store_cred_secret(db, cfg.device_id, cfg.credential_secret.value());

          return AstarteDeviceSdk::MqttConfig::with_credential_secret(
              cfg.realm, cfg.device_id, cfg.credential_secret.value(),
              std::format("{}/pairing", cfg.astarte_base_url), cfg.store_dir);
        } else {
          return AstarteDeviceSdk::MqttConfig::with_pairing_token(
              cfg.realm, cfg.device_id, cfg.pairing_token.value(),
              std::format("{}/pairing", cfg.astarte_base_url), cfg.store_dir);
        }
      }();

      // here you can modify the mqtt_cfg options, such as the keepalive interval, the connection
      // timeout period, etc.

      auto device = AstarteDeviceSdk::AstarteDeviceMQTT(mqtt_cfg);

      std::filesystem::path device_aggregate_interface_file_path =
          "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.DeviceAggregate.json";
      device.add_interface_from_file(device_aggregate_interface_file_path);

      device.connect();

      device.disconnect();
    }
  } catch (const std::exception& e) {
    spdlog::error("Exception thown: {}", e.what());
  }

  return 0;
}
