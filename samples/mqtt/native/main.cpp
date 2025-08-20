// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <SQLiteCpp/SQLiteCpp.h>
#include <spdlog/spdlog.h>

#include <string_view>

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

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::debug);

  auto cfg = Config("samples/mqtt/native/config.toml");

  try {
    auto api = AstarteDeviceSdk::PairingApi(cfg.realm, cfg.device_id, cfg.pairing_url);

    if (cfg.features.registration_enabled()) {
      auto db = init_db("samples/mqtt/native/example.db");

      // Check if the device is already registered
      SQLite::Statement query(db,
                              "SELECT credential_secret FROM registered_devices WHERE device_id=?");
      query.bind(1, cfg.device_id);

      if (query.executeStep()) {
        spdlog::debug("device {} already registered with credential secret {}", cfg.device_id,
                      query.getColumn(0).getString());
        return 0;
      }

      auto secret = api.register_device(cfg.pairing_token.value());
      spdlog::info("credential secret: {}", secret);

      // Add the registered device id
      SQLite::Statement query_insert(db, "INSERT INTO registered_devices VALUES (?, ?)");
      query_insert.bind(1, cfg.device_id);
      query_insert.bind(2, secret);
      int nb = query_insert.exec();
      spdlog::debug("{} entry stored in db", nb);

      auto broker_url = api.get_broker_url(secret);
      spdlog::info("broker url: {}", broker_url);
    }
  } catch (const std::exception& e) {
    spdlog::error("Exception thown: {}", e.what());
  }

  return 0;
}
