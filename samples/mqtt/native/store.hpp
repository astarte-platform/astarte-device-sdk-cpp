// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <SQLiteCpp/SQLiteCpp.h>
#include <spdlog/spdlog.h>

#include <optional>
#include <string>

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

auto credential_secret_from_db(SQLite::Database& db, std::string device_id)
    -> std::optional<std::string> {
  // Check if the device is already registered
  SQLite::Statement query(db, "SELECT credential_secret FROM registered_devices WHERE device_id=?");
  query.bind(1, device_id);

  if (!query.executeStep()) {
    return std::nullopt;
  }

  auto cred = query.getColumn(0).getString();
  spdlog::debug("device {} already registered with credential secret {}", device_id, cred);

  return std::move(std::optional(cred));
}

void store_cred_secret(SQLite::Database& db, std::string device_id, std::string secret) {
  SQLite::Statement query_insert(db, "INSERT INTO registered_devices VALUES (?, ?)");
  query_insert.bind(1, device_id);
  query_insert.bind(2, secret);
  query_insert.exec();
  spdlog::debug("credential secret stored in db");
}
