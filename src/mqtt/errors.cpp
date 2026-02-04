// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/errors.hpp"

#include <string_view>
#include <variant>

#include "astarte_device_sdk/errors.hpp"

namespace astarte::device::mqtt {

MqttError::MqttError(std::string_view message) : ErrorBase(k_type_, message) {}
MqttError::MqttError() : ErrorBase(k_type_, "") {}
MqttError::MqttError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}
MqttError::MqttError(const Error& other)
    : ErrorBase(k_type_, "",
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

PairingApiError::PairingApiError(std::string_view message) : ErrorBase(k_type_, message) {}
PairingApiError::PairingApiError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InvalidUrlError::InvalidUrlError(std::string_view message) : ErrorBase(k_type_, message) {}
InvalidUrlError::InvalidUrlError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

DeviceRegistrationError::DeviceRegistrationError(std::string_view message)
    : ErrorBase(k_type_, message) {}
DeviceRegistrationError::DeviceRegistrationError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

JsonParsingError::JsonParsingError(std::string_view message) : ErrorBase(k_type_, message) {}
JsonParsingError::JsonParsingError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

RetrieveBrokerUrlError::RetrieveBrokerUrlError(std::string_view message)
    : ErrorBase(k_type_, message) {}
RetrieveBrokerUrlError::RetrieveBrokerUrlError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

ReadCredentialError::ReadCredentialError(std::string_view message) : ErrorBase(k_type_, message) {}
ReadCredentialError::ReadCredentialError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

WriteCredentialError::WriteCredentialError(std::string_view message)
    : ErrorBase(k_type_, message) {}
WriteCredentialError::WriteCredentialError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

PairingConfigError::PairingConfigError(std::string_view message) : ErrorBase(k_type_, message) {}
PairingConfigError::PairingConfigError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

CryptoError::CryptoError(std::string_view message) : ErrorBase(k_type_, message) {}
CryptoError::CryptoError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

UuidError::UuidError(std::string_view message) : ErrorBase(k_type_, message) {}
UuidError::UuidError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

HttpError::HttpError(std::string_view message) : ErrorBase(k_type_, message) {}
HttpError::HttpError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

MqttConnectionError::MqttConnectionError(std::string_view message) : ErrorBase(k_type_, message) {}
MqttConnectionError::MqttConnectionError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}
}  // namespace astarte::device::mqtt
