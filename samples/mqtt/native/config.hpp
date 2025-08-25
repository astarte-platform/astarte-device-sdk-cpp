// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>
#include <optional>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>

class Modes {
 public:
  // check whether the registration mode is enabled
  bool registration_enabled() {
    return (active_modes & modes::REGISTRATION == modes::REGISTRATION) ? true : false;
  }

  void set_registration_mode() { active_modes = modes::REGISTRATION; }

  int active_modes;

 private:
  // use this enum as bitmask to obtain the active modes.
  enum modes {
    REGISTRATION = 1,
  };
};

class Config {
 public:
  Config(std::string_view file) {
    toml::table toml;
    try {
      toml = toml::parse_file(file);

      this->general_config(*this, toml);

      this->mode_config(*this, toml);

      if (this->modes.registration_enabled()) {
        this->mode_registration_config(*this, toml);
      }
    } catch (const toml::parse_error& err) {
      spdlog::error("Config file parsing failed due to {}", err.what());
      throw err;
    }
  }

  std::string pairing_url;
  std::string realm;
  std::string device_id;
  std::optional<std::string> pairing_jwt;

  Modes modes;

  private:
  void general_config(Config& cfg, toml::table& toml) {
    try {
      cfg.pairing_url = toml.at("general").at_path("pairing_url").value<std::string>().value();
      cfg.realm = toml.at("general").at_path("realm").value<std::string>().value();
      cfg.device_id = toml.at("general").at_path("device_id").value<std::string>().value();
    } catch (const toml::parse_error& err) {
      throw err;
    }
  }

  void mode_config(Config& cfg, toml::table& toml) {
    try {
      bool registration_enabled =
      toml.at("mode").at_path("DEVICE_REGISTRATION").value<bool>().value();

      if (registration_enabled) {
        modes.set_registration_mode();
      }
    } catch (const toml::parse_error& err) {
      throw err;
    }
  }

  void mode_registration_config(Config& cfg, toml::table& toml) {
    try {
      assert(cfg.modes.registration_enabled());

      cfg.pairing_jwt = toml.at("mode")
      .at_path("registration")
      .at_path("pairing_jwt")
      .value<std::string>();
    } catch (const toml::parse_error& err) {
      throw err;
    }
  }
};
