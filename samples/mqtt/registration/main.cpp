// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <string_view>

#include "astarte_device_sdk/mqtt/pairing.hpp"

using json = nlohmann::json;

struct Config {
  std::string pairing_jwt;
  std::string pairing_url;
  std::string realm;
  std::string device_id;
};

void from_json(const json& j, Config& c) {
  j.at("pairing_jwt").get_to(c.pairing_jwt);
  j.at("pairing_url").get_to(c.pairing_url);
  j.at("realm").get_to(c.realm);
  j.at("device_id").get_to(c.device_id);
}

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::debug);

  std::ifstream file("samples/mqtt/registration/config.json");
  if (!file.is_open()) {
    spdlog::error("Error: Could not open config.json.");
    return 1;
  }
  json json = json::parse(file);
  auto cfg = json.template get<Config>();

  try {
    auto api = AstarteDeviceSdk::ApiClient(std::move(cfg.realm), std::move(cfg.device_id),
                                           std::move(cfg.pairing_url));

    auto secret = api.register_device(cfg.pairing_jwt);
    spdlog::info("credential secret: {}", secret);

    auto broker_url = api.get_broker_url(secret);
    spdlog::info("broker url: {}", broker_url);

    auto device_cert = api.get_device_cert(secret);
    spdlog::info("device certificate: {}", device_cert);

    auto valid_cert = api.device_cert_valid(device_cert, secret);
    spdlog::info("device certificate is {}valid", valid_cert ? "" : "not ");
  } catch (const std::exception& e) {
    spdlog::error("Exception thown: {}", e.what());
  }

  return 0;
}
