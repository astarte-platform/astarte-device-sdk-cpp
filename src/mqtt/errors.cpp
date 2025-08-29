// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/errors.hpp"

#include <string_view>
#include <variant>

#include "astarte_device_sdk/errors.hpp"

namespace AstarteDeviceSdk {

AstarteMqttError::AstarteMqttError(std::string_view message) : AstarteErrorBase(k_type_, message) {}
AstarteMqttError::AstarteMqttError() : AstarteErrorBase(k_type_, "") {}
AstarteMqttError::AstarteMqttError(std::string_view message, const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}
AstarteMqttError::AstarteMqttError(const AstarteError& other)
    : AstarteErrorBase(
          k_type_, "",
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstartePairingApiError::AstartePairingApiError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstartePairingApiError::AstartePairingApiError(std::string_view message, const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteInvalidUrlError::AstarteInvalidUrlError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteInvalidUrlError::AstarteInvalidUrlError(std::string_view message, const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteDeviceRegistrationError::AstarteDeviceRegistrationError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteDeviceRegistrationError::AstarteDeviceRegistrationError(std::string_view message,
                                                               const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteJsonParsingError::AstarteJsonParsingError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteJsonParsingError::AstarteJsonParsingError(std::string_view message,
                                                 const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteRetrieveBrokerUrlError::AstarteRetrieveBrokerUrlError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteRetrieveBrokerUrlError::AstarteRetrieveBrokerUrlError(std::string_view message,
                                                             const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteReadCredentialError::AstarteReadCredentialError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteReadCredentialError::AstarteReadCredentialError(std::string_view message,
                                                       const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteWriteCredentialError::AstarteWriteCredentialError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteWriteCredentialError::AstarteWriteCredentialError(std::string_view message,
                                                         const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstartePairingConfigError::AstartePairingConfigError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstartePairingConfigError::AstartePairingConfigError(std::string_view message,
                                                     const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteCryptoError::AstarteCryptoError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteCryptoError::AstarteCryptoError(std::string_view message, const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteUuidError::AstarteUuidError(std::string_view message) : AstarteErrorBase(k_type_, message) {}
AstarteUuidError::AstarteUuidError(std::string_view message, const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteHttpError::AstarteHttpError(std::string_view message) : AstarteErrorBase(k_type_, message) {}
AstarteHttpError::AstarteHttpError(std::string_view message, const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteMqttConnectionError::AstarteMqttConnectionError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteMqttConnectionError::AstarteMqttConnectionError(std::string_view message,
                                                       const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}
}  // namespace AstarteDeviceSdk
