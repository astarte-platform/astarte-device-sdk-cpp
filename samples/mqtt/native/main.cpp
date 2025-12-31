// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include <format>
#include <optional>
#include <string>
#include <string_view>

#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "config.hpp"
#include "store.hpp"

int main() {
  spdlog::set_level(spdlog::level::debug);

  auto cfg = Config("samples/mqtt/native/config.toml");

  auto api_res =
      AstarteDeviceSdk::PairingApi::create(cfg.realm, cfg.device_id, cfg.astarte_base_url);
  if (!api_res) {
    spdlog::error("Pairing API creation failure. Err: \n{}", api_res.error());
    return 0;
  }
  auto api = api_res.value();

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

    auto secret_res = api.register_device(cfg.pairing_token.value());
    if (!secret_res) {
      spdlog::error("failed to register the device: {}", secret_res.error());
      return 1;
    }
    auto secret = secret_res.value();
    spdlog::info("credential secret: {}", secret);

    auto key_cert_res = api.get_device_key_and_cert(secret);
    if (!key_cert_res) {
      spdlog::error("failed to get the device key or cert: {}", key_cert_res.error());
      return 1;
    }
    auto [key, cert] = key_cert_res.value();
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

    auto mqtt_cfg = [&] {
      if (in_db) {
        return config::MqttConfig::with_credential_secret(
            cfg.realm, cfg.device_id, *cred_opt,
            astarte_fmt::format("{}/pairing", cfg.astarte_base_url), cfg.store_dir);
      } else if (cfg.credential_secret) {
        // first, store the cred secret in the db for future usage
        store_cred_secret(db, cfg.device_id, cfg.credential_secret.value());

        return config::MqttConfig::with_credential_secret(
            cfg.realm, cfg.device_id, cfg.credential_secret.value(),
            astarte_fmt::format("{}/pairing", cfg.astarte_base_url), cfg.store_dir);
      } else {
        return config::MqttConfig::with_pairing_token(
            cfg.realm, cfg.device_id, cfg.pairing_token.value(),
            astarte_fmt::format("{}/pairing", cfg.astarte_base_url), cfg.store_dir);
      }
    }();

    // here you can modify the mqtt_cfg options, such as the keepalive interval, the connection
    // timeout period, etc.

    auto device_res = AstarteDeviceSdk::AstarteDeviceMqtt::create(std::move(mqtt_cfg));
    if (!device_res) {
      spdlog::error("device creation error: {}", device_res.error());
      return 1;
    }
    auto device = *std::move(device_res);

    const std::vector<std::string_view> interfaces = {
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.DeviceAggregate.json",
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.DeviceDatastream.json",
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.DeviceProperty.json",
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.ServerAggregate.json",
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.ServerDatastream.json",
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.ServerProperty.json"};

    for (const auto interface : interfaces) {
      auto add_interface_res = device.add_interface_from_file(interface);
      if (!add_interface_res) {
        spdlog::error("Failed to add interface {}. Error: {}", interface,
                      add_interface_res.error());
        return 1;
      }
    }

    auto conn_res = device.connect();
    if (!conn_res) {
      spdlog::error("connection error: {}", conn_res.error());
      return 1;
    }

    sleep(10);

    auto disconn_res = device.disconnect();
    if (!disconn_res) {
      spdlog::error("connection error: {}", disconn_res.error());
      return 1;
    }
  }

  return 0;
}
