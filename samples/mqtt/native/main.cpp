// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <SQLiteCpp/SQLiteCpp.h>
#include <spdlog/spdlog.h>

#include <format>
#include <string_view>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/connection.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "config.hpp"

auto init_db(std::string_view db_path) -> SQLite::Database {
  // Open db or create it
  SQLite::Database db(db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  spdlog::debug("SQLite database file '{}' opened successfully", db.getFilename().c_str());

  // Create a new table with an explicit "id" column aliasing the underlying rowid
  db.exec(
      "CREATE TABLE IF NOT EXISTS registered_devices (device_id STRING PRIMARY KEY, "
      "credential_secret STRING)");

  return std::move(db);
}

int main() {
  spdlog::set_level(spdlog::level::debug);

  auto cfg = Config("samples/mqtt/native/config.toml");

  try {
    auto api = AstarteDeviceSdk::PairingApi(cfg.realm, cfg.device_id, cfg.astarte_base_url);
    auto db = init_db("samples/mqtt/native/example.db");

    if (cfg.features.registration_enabled()) {
      // Check if the device is already registered
      SQLite::Statement query(db,
                              "SELECT credential_secret FROM registered_devices WHERE device_id=?");
      query.bind(1, cfg.device_id);

      if (query.executeStep()) {
        spdlog::debug("device {} already registered with credential secret {}", cfg.device_id,
                      query.getColumn(0).getString());
      } else {
        assert(cfg.pairing_token);

        auto secret = api.register_device(cfg.pairing_token.value());
        spdlog::info("credential secret: {}", secret);
        auto [key, cert] = api.get_device_key_and_cert(secret);
        spdlog::info("key: {}", key);
        spdlog::info("cert: {}", cert);

        // Add the registered device id
        SQLite::Statement query_insert(db, "INSERT INTO registered_devices VALUES (?, ?)");
        query_insert.bind(1, cfg.device_id);
        query_insert.bind(2, secret);
        int nb = query_insert.exec();
        spdlog::debug("{} entry stored in db", nb);
      }
    }

    if (cfg.features.connection_enabled()) {
      if (!cfg.pairing_token && !cfg.credential_secret) {
        spdlog::error("neither pairing token nor credential secret are set");
        return 1;
      }

      const auto mqtt_cfg = [&] {
        if (cfg.pairing_token) {
          return AstarteDeviceSdk::MqttConfig::with_pairing_token(
              cfg.realm, cfg.device_id, cfg.pairing_token.value(),
              std::format("{}/pairing", cfg.astarte_base_url), cfg.store_dir);
        } else {
          return AstarteDeviceSdk::MqttConfig::with_credential_secret(
              cfg.realm, cfg.device_id, cfg.credential_secret.value(),
              std::format("{}/pairing", cfg.astarte_base_url), cfg.store_dir);
        }
      }();

      auto conn = AstarteDeviceSdk::MqttConnection(std::move(mqtt_cfg));
      conn.connect();
      sleep(3);
      conn.disconnect();
    }
  } catch (const std::exception& e) {
    spdlog::error("Exception thown: {}", e.what());
  }

  return 0;
}
