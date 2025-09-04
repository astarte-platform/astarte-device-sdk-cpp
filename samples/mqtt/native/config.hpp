// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>

#include "ada.h"

class Features {
 public:
  // check whether the registration feature is enabled
  bool registration_enabled() {
    return (active_features & features::REGISTRATION) == features::REGISTRATION ? true : false;
  }

  void set_registration_feature() { active_features |= features::REGISTRATION; }

  // check whether the connection feature is enabled
  bool connection_enabled() {
    return (active_features & features::CONNECTION) == features::CONNECTION ? true : false;
  }

  void set_connection_feature() { active_features |= features::CONNECTION; }

  int active_features;

 private:
  // use this enum as bitmask to obtain the active features.
  enum features {
    REGISTRATION = (1u << 0),
    CONNECTION = (1u << 1),
  };
};

class Config {
 public:
  Config(std::string_view file) {
    toml::table toml;
    try {
      toml = toml::parse_file(file);

      this->general_config(*this, toml);
      this->feature_config(*this, toml);
    } catch (const std::exception& err) {
      spdlog::error("Config file parsing failed due to {}", err.what());
      throw err;
    }
  }

  std::string astarte_base_url;
  std::string realm;
  std::string device_id;
  std::string store_dir;
  std::optional<std::string> pairing_token;
  std::optional<std::string> credential_secret;

  Features features;

 private:
  void general_config(Config& cfg, toml::table& toml) {
    auto cfg_astarte_base_url = toml.at("astarte_base_url").value<std::string>().value();
    auto astarte_base_url = ada::parse(cfg_astarte_base_url).value();

    cfg.astarte_base_url = astarte_base_url.get_href();
    cfg.realm = toml.at("realm").value<std::string>().value();
    cfg.device_id = toml.at("device_id").value<std::string>().value();
    cfg.store_dir = toml.at("store_dir").value<std::string>().value();
    cfg.pairing_token = toml.at_path("pairing_token").value<std::string>();
    cfg.credential_secret = toml.at_path("credential_secret").value<std::string>();
  }

  void feature_config(Config& cfg, toml::table& toml) {
    bool registration_enabled =
        toml.at("features").at_path("DEVICE_REGISTRATION").value<bool>().value();
    spdlog::debug("registration flag {}enabled", registration_enabled ? "" : "not ");

    bool connection_enabled =
        toml.at("features").at_path("DEVICE_CONNECTION").value<bool>().value();
    spdlog::debug("connection flag {}enabled", connection_enabled ? "" : "not ");

    if (registration_enabled) {
      features.set_registration_feature();
    }

    if (connection_enabled) {
      features.set_connection_feature();
    }
  }
};
