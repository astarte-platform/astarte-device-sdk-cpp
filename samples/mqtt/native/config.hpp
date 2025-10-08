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
    return (active_features & features::REGISTRATION == features::REGISTRATION) ? true : false;
  }

  void set_registration_feature() { active_features |= features::REGISTRATION; }

  int active_features;

 private:
  // use this enum as bitmask to obtain the active features.
  enum features {
    REGISTRATION = (1u << 0),
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

  std::string pairing_url;
  std::string realm;
  std::string device_id;
  std::optional<std::string> pairing_token;

  Features features;

 private:
  void general_config(Config& cfg, toml::table& toml) {
    auto astarte_base_url = toml.at("astarte_base_url").value<std::string>().value();
    auto pairing_url = ada::parse(astarte_base_url).value();
    pairing_url.set_pathname("pairing");

    cfg.pairing_url = pairing_url.get_href();
    cfg.realm = toml.at("realm").value<std::string>().value();
    cfg.device_id = toml.at("device_id").value<std::string>().value();
    cfg.pairing_token = toml.at("pairing_token").value<std::string>();
  }

  void feature_config(Config& cfg, toml::table& toml) {
    bool registration_enabled =
        toml.at("features").at_path("DEVICE_REGISTRATION").value<bool>().value();

    if (registration_enabled) {
      features.set_registration_feature();
    }
  }
};
