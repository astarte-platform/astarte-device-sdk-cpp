// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"

using AstarteDeviceSdk::PairingApi;

constexpr size_t CREDENTIAL_SECRET_LEN = 44;

class TestActionPairingApiRegistration : public TestAction {
 public:
  static std::shared_ptr<TestActionPairingApiRegistration> Create(std::string pairing_token) {
    return std::shared_ptr<TestActionPairingApiRegistration>(
        new TestActionPairingApiRegistration(pairing_token));
  }

  void execute_unchecked(const std::string& case_name) const override {
    spdlog::info("[{}] Pairing device...", case_name);
    auto pairing_api = PairingApi(realm_, device_id_, std::format("{}/pairing", astarte_base_url_));
    auto credential_secret = pairing_api.register_device(pairing_token_);

    // check that the credential secret has the correct lenght
    assert(credential_secret.length() == CREDENTIAL_SECRET_LEN);
  }

 private:
  TestActionPairingApiRegistration(std::string pairing_token) : pairing_token_(pairing_token) {}
  std::string pairing_token_;
};
