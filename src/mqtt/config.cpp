// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/config.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>

#include "ada.h"
#include "astarte_device_sdk/mqtt/connection.hpp"
#include "astarte_device_sdk/mqtt/exceptions.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "mqtt/connect_options.h"

namespace AstarteDeviceSdk {

auto read_credential_from_file(const std::filesystem::path& file_path)
    -> std::optional<std::string> {
  if(!std::filesystem::exists(file_path)) {
    spdlog::debug("file {} does not exists", file_path.string());
    return std::nullopt;
  }

  std::ifstream interface_file(file_path, std::ios::in);
  if (!interface_file.is_open()) {
    spdlog::debug("Could not open the credential file: {}", file_path.string());
    return std::nullopt;
  }

  // read the entire file content into a string
  const std::string credential((std::istreambuf_iterator<char>(interface_file)),
                               std::istreambuf_iterator<char>());
  interface_file.close();

  return credential;
}

void write_to_file(const std::filesystem::path& file_path, const std::string data) {
  if(std::filesystem::exists(file_path)) {
    spdlog::debug("file {} already exists", file_path.string());
    return;
  }

  // open an output file stream (ofstream) using the path object
  // the file is automatically closed when 'output_file' goes out of scope
  std::ofstream output_file(file_path);

  // error if the file was not opened successfully
  if (!output_file.is_open()) {
    throw WriteCredentialException(std::format("couldn't open file {}", file_path.string()));
  }

  output_file << data;
}

auto MqttConfig::read_secret_or_register() -> std::string {
  if (credential_.is_credential_secret()) {
    return credential_.value();
  }

  auto api = AstarteDeviceSdk::PairingApi(realm_, device_id_, pairing_url_);
  auto credential_secret = api.register_device(credential_.value());

  // substitute the pairing token with the credential secret in the internal configuration
  credential_ = Credential::secret(credential_secret);

  return credential_secret;
}

auto MqttConfig::build_mqtt_options() -> mqtt::connect_options {
  auto conn_opts = mqtt::connect_options_builder::v3();

  if (keepalive_ <= conn_timeout_) {
    throw PairingConfigException(
        std::format("Keep alive ({}s) should be greater than the connection timeout ({}s)",
                    keepalive_, conn_timeout_));
  }

  conn_opts.keep_alive_interval(std::chrono::seconds(keepalive_))
      .connect_timeout(std::chrono::seconds(conn_timeout_))
      .clean_session();

  auto ssl_opts =
      mqtt::ssl_options_builder()
          .ssl_version(3)  // TLS 1.2
          // TODO: enable for server authentication
          .enable_server_cert_auth(false)
          .verify(false)
          // Astarte MQTT broker requires client authentication (mutual TLS),
          .key_store(std::format("{}/{}", store_dir_, CLIENT_CERTIFICATE_FILE))
          .private_key(std::format("{}/{}", store_dir_, PRIVATE_KEY_FILE))
          .error_handler([](const std::string& msg) { spdlog::error("TLS error: {}", msg); })
          .finalize();

  // Add the SSL options to the connection builder
  conn_opts.ssl(std::move(ssl_opts));

  return conn_opts.finalize();
}

}  // namespace AstarteDeviceSdk
