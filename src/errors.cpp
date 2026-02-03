// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/errors.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <variant>

namespace astarte::device {

ErrorBase::ErrorBase(std::string_view type, std::string_view message)
    : type_(type), message_(message), other_(nullptr) {}
ErrorBase::ErrorBase(std::string_view type, std::string_view message, const ErrorBase& other)
    : type_(type), message_(message), other_(std::make_shared<ErrorBase>(other)) {}
auto ErrorBase::message() const -> const std::string& { return message_; }
auto ErrorBase::type() const -> const std::string& { return type_; }
auto ErrorBase::nested_error() const -> const std::shared_ptr<ErrorBase>& { return other_; }

DataSerializationError::DataSerializationError(std::string_view message)
    : ErrorBase(k_type_, message) {}
DataSerializationError::DataSerializationError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InternalError::InternalError(std::string_view message) : ErrorBase(k_type_, message) {}
InternalError::InternalError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

FileOpenError::FileOpenError(std::string_view filename) : ErrorBase(k_type_, filename) {}
FileOpenError::FileOpenError(std::string_view filename, const Error& other)
    : ErrorBase(k_type_, filename,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InvalidInputError::InvalidInputError(std::string_view message) : ErrorBase(k_type_, message) {}
InvalidInputError::InvalidInputError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

OperationRefusedError::OperationRefusedError(std::string_view message)
    : ErrorBase(k_type_, message) {}
OperationRefusedError::OperationRefusedError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

GrpcLibError::GrpcLibError(std::string_view message) : ErrorBase(k_type_, message) {}
GrpcLibError::GrpcLibError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}
GrpcLibError::GrpcLibError(std::uint64_t code, std::string_view message)
    : ErrorBase(k_type_,
                "code(" + std::to_string(code) + ")-message(" + std::string(message) + ")") {}
GrpcLibError::GrpcLibError(std::uint64_t code, std::string_view message, const Error& other)
    : ErrorBase(k_type_, "code(" + std::to_string(code) + ")-message(" + std::string(message) + ")",
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

MsgHubError::MsgHubError(std::string_view message) : ErrorBase(k_type_, message) {}
MsgHubError::MsgHubError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InvalidInterfaceTypeError::InvalidInterfaceTypeError(std::string_view message)
    : ErrorBase(k_type_, message) {}
InvalidInterfaceTypeError::InvalidInterfaceTypeError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InterfaceValidationError::InterfaceValidationError(std::string_view message)
    : ErrorBase(k_type_, message) {}
InterfaceValidationError::InterfaceValidationError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InvalidInterfaceVersionError::InvalidInterfaceVersionError(std::string_view message)
    : ErrorBase(k_type_, message) {}
InvalidInterfaceVersionError::InvalidInterfaceVersionError(std::string_view message,
                                                           const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InvalidInterfaceOwnershipeError::InvalidInterfaceOwnershipeError(std::string_view message)
    : ErrorBase(k_type_, message) {}
InvalidInterfaceOwnershipeError::InvalidInterfaceOwnershipeError(std::string_view message,
                                                                 const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InvalidInterfaceAggregationError::InvalidInterfaceAggregationError(std::string_view message)
    : ErrorBase(k_type_, message) {}
InvalidInterfaceAggregationError::InvalidInterfaceAggregationError(std::string_view message,
                                                                   const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InvalidAstarteTypeError::InvalidAstarteTypeError(std::string_view message)
    : ErrorBase(k_type_, message) {}
InvalidAstarteTypeError::InvalidAstarteTypeError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InvalidReliabilityError::InvalidReliabilityError(std::string_view message)
    : ErrorBase(k_type_, message) {}
InvalidReliabilityError::InvalidReliabilityError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InvalidRetentionError::InvalidRetentionError(std::string_view message)
    : ErrorBase(k_type_, message) {}
InvalidRetentionError::InvalidRetentionError(std::string_view message, const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}

InvalidDatabaseRetentionPolicyError::InvalidDatabaseRetentionPolicyError(std::string_view message)
    : ErrorBase(k_type_, message) {}
InvalidDatabaseRetentionPolicyError::InvalidDatabaseRetentionPolicyError(std::string_view message,
                                                                         const Error& other)
    : ErrorBase(k_type_, message,
                std::visit([](const auto& err) -> const ErrorBase& { return err; }, other)) {}
}  // namespace astarte::device
