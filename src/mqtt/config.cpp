// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/config.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <format>
#include <string>
#include <utility>

#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "mqtt/connect_options.h"

namespace AstarteDeviceSdk {

namespace config {

#include <unistd.h>

#include <cstdio>
#include <system_error>
#include <vector>

auto read_from_file(const std::filesystem::path& file_path)
    -> astarte_tl::expected<std::string, AstarteError> {
  std::ifstream interface_file(file_path, std::ios::in);
  if (!interface_file.is_open()) {
    return astarte_tl::unexpected(AstarteReadCredentialError(
        astarte_fmt::format("Could not open the credential file: {}", file_path.string())));
  }

  // read the entire file content into a string
  std::string data((std::istreambuf_iterator<char>(interface_file)),
                   std::istreambuf_iterator<char>());
  interface_file.close();

  return data;
}

auto write_to_file(const std::filesystem::path& file_path, std::string_view data)
    -> astarte_tl::expected<void, AstarteError> {
  // open an output file stream (ofstream) using the path object
  // the file is automatically closed when 'output_file' goes out of scope
  std::ofstream output_file(file_path);

  // error if the file was not opened successfully
  if (!output_file.is_open()) {
    return astarte_tl::unexpected(AstarteWriteCredentialError(
        astarte_fmt::format("couldn't open file {}", file_path.string())));
  }

  output_file << data;

  return {};
}

auto secure_shred_file(const std::string& path) -> astarte_tl::expected<void, AstarteError> {
  FILE* f = std::fopen(path.c_str(), "rb+");
  if (!f) {
    // capture the global 'errno', wrap it in an error_code, and return as unexpected
    return astarte_tl::unexpected(AstarteWriteCredentialError(astarte_fmt::format(
        "failed to open the file, {}", std::error_code(errno, std::generic_category()).message())));
  }

  // get size
  std::fseek(f, 0, SEEK_END);
  long size = std::ftell(f);
  std::rewind(f);

  // write zeros
  const size_t buf_size = 4096;
  std::vector<unsigned char> buf(buf_size, 0);
  long written = 0;

  while (written < size) {
    long to_write = (size - written > buf_size) ? buf_size : (size - written);
    size_t write_count = std::fwrite(buf.data(), 1, to_write, f);

    // check if write failed
    if (write_count != to_write) {
      std::fclose(f);
      return astarte_tl::unexpected(AstarteWriteCredentialError(
          astarte_fmt::format("failed to write zeros to file, {}",
                              std::error_code(errno, std::generic_category()).message())));
    }
    written += to_write;
  }

  // flush and sync
  std::fflush(f);
  if (fsync(fileno(f)) != 0) {
    std::fclose(f);
    return astarte_tl::unexpected(AstarteWriteCredentialError(
        astarte_fmt::format("failed to flush and sync modifications, {}",
                            std::error_code(errno, std::generic_category()).message())));
  }

  std::fclose(f);

  // delete the file
  std::error_code ec;
  std::filesystem::remove(path, ec);
  if (ec) {
    return astarte_tl::unexpected(AstarteWriteCredentialError(
        astarte_fmt::format("failed to delete the file, {}", ec.message())));
  }

  return {};
}

}  // namespace config

auto MqttConfig::build_mqtt_options() -> astarte_tl::expected<mqtt::connect_options, AstarteError> {
  auto conn_opts = mqtt::connect_options_builder::v3();

  if (keepalive_ <= conn_timeout_) {
    return astarte_tl::unexpected(AstartePairingConfigError(
        astarte_fmt::format("Keep alive ({}s) should be greater than the connection timeout ({}s)",
                            keepalive_, conn_timeout_)));
  }

  conn_opts.keep_alive_interval(std::chrono::seconds(keepalive_))
      .connect_timeout(std::chrono::seconds(conn_timeout_))
      .automatic_reconnect(std::chrono::seconds(2), std::chrono::minutes(1))
      .clean_session(true);

  auto ssl_opts =
      mqtt::ssl_options_builder()
          .ssl_version(3)
          .enable_server_cert_auth(true)
          .verify(false)
          // Astarte MQTT broker requires client authentication (mutual TLS),
          .key_store(astarte_fmt::format("{}/{}", store_dir_, CLIENT_CERTIFICATE_FILE))
          .private_key(astarte_fmt::format("{}/{}", store_dir_, PRIVATE_KEY_FILE))
          .error_handler([](const std::string& msg) { spdlog::error("TLS error: {}", msg); })
          .finalize();

  // Add the SSL options to the connection builder
  conn_opts.ssl(std::move(ssl_opts));

  return conn_opts.finalize();
}

}  // namespace AstarteDeviceSdk
