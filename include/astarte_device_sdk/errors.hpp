// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_ERRORS_H
#define ASTARTE_DEVICE_SDK_ERRORS_H

/**
 * @file astarte_device_sdk/errors.hpp
 * @brief Error types and handling for the Astarte device library.
 *
 * @details This file defines the comprehensive error hierarchy used across the SDK,
 * utilizing a variant-based approach for type-safe error propagation and nesting
 * without relying on C++ exceptions.
 */

#if defined(ASTARTE_USE_TL_EXPECTED)
#include <tl/expected.hpp>
#else
#include <expected>
#endif

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

#if defined(ASTARTE_USE_TL_EXPECTED)
namespace astarte_tl = ::tl;
#else
namespace astarte_tl = ::std;
#endif

class DataSerializationError;
class FileOpenError;
class InvalidInputError;
class InternalError;
class OperationRefusedError;
class GrpcLibError;
class MsgHubError;
class InterfaceValidationError;
class InvalidInterfaceVersionError;
class InvalidInterfaceTypeError;
class InvalidInterfaceOwnershipeError;
class InvalidInterfaceAggregationError;
class InvalidReliabilityError;
class InvalidAstarteTypeError;
class InvalidRetentionError;
class InvalidDatabaseRetentionPolicyError;
#if !defined(ASTARTE_TRANSPORT_GRPC)
namespace mqtt {
class JsonParsingError;
class DeviceRegistrationError;
class PairingApiError;
class MqttError;
class InvalidUrlError;
class RetrieveBrokerUrlError;
class ReadCredentialError;
class WriteCredentialError;
class PairingConfigError;
class CryptoError;
class UuidError;
class HttpError;
class MqttConnectionError;
}  // namespace mqtt
#endif

/**
 * @brief Variant type representing any error from the Astarte device library.
 *
 * @details Intended for use as the error type `E` in `astarte_tl::expected<T, E>`.
 * This allows for efficient, stack-allocated error reporting across the SDK.
 */
using Error =
    std::variant<DataSerializationError, InternalError, FileOpenError, InvalidInputError,
                 InterfaceValidationError, InvalidInterfaceVersionError, InvalidInterfaceTypeError,
                 InvalidInterfaceOwnershipeError, InvalidInterfaceAggregationError,
                 InvalidAstarteTypeError, InvalidReliabilityError, InvalidRetentionError,
                 InvalidDatabaseRetentionPolicyError,
#if !defined(ASTARTE_TRANSPORT_GRPC)
                 OperationRefusedError, GrpcLibError, MsgHubError, mqtt::JsonParsingError,
                 mqtt::DeviceRegistrationError, mqtt::PairingApiError, mqtt::MqttError,
                 mqtt::InvalidUrlError, mqtt::RetrieveBrokerUrlError, mqtt::ReadCredentialError,
                 mqtt::WriteCredentialError, mqtt::PairingConfigError, mqtt::CryptoError,
                 mqtt::UuidError, mqtt::HttpError, mqtt::MqttConnectionError>;
#else
                 OperationRefusedError, GrpcLibError, MsgHubError>;
#endif

/**
 * @brief Base error class representing any possible error from the Astarte device library.
 *
 * @details Provides the fundamental structure for error reporting, including type
 * identification, human-readable messages, and support for nested (wrapped) errors.
 */
class ErrorBase {
 public:
  /// @brief Destructor for the Astarte error.
  virtual ~ErrorBase() = default;

  /// @brief Default copy constructor.
  ErrorBase(const ErrorBase&) = default;

  /**
   * @brief Default copy assignment operator.
   * @return A reference to the assigned class instance.
   */
  auto operator=(const ErrorBase&) -> ErrorBase& = default;

  /** @brief Delete move constructor. */
  ErrorBase(ErrorBase&&) = delete;

  /**
   * @brief Default move assignment operator.
   * @return A reference to the assigned instance.
   */
  auto operator=(ErrorBase&&) -> ErrorBase& = default;

  /**
   * @brief Retrieves the error message.
   * @return A constant reference to the error message string.
   */
  [[nodiscard]] auto message() const -> const std::string&;

  /**
   * @brief Retrieves the error type string.
   * @return A constant reference to the error type string.
   */
  [[nodiscard]] auto type() const -> const std::string&;

  /**
   * @brief Retrieves the nested error, if any.
   * @return A constant reference to a shared pointer to the nested error, or nullptr if none.
   */
  [[nodiscard]] auto nested_error() const -> const std::shared_ptr<ErrorBase>&;

 protected:
  /**
   * @brief Constructor for the Astarte error.
   * @param[in] type Error type name.
   * @param[in] message Error message.
   */
  explicit ErrorBase(std::string_view type, std::string_view message);

  /**
   * @brief Wrapping constructor for the Astarte error.
   * @param[in] type Error type name.
   * @param[in] message Error message.
   * @param[in] other Nested error to wrap.
   */
  explicit ErrorBase(std::string_view type, std::string_view message, const ErrorBase& other);

 private:
  std::string type_;
  std::string message_;
  std::shared_ptr<ErrorBase> other_;
};

/// @brief Specific error for when a serialization operation failed.
class DataSerializationError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message Human-readable error message.
   */
  explicit DataSerializationError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message Human-readable error message.
   * @param[in] other The underlying Error to nest.
   */
  explicit DataSerializationError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "DataSerializationError";
};

/// @brief Specific error for when an operation failed due to an internal error.
class InternalError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message Human-readable error message.
   */
  explicit InternalError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message Human-readable error message.
   * @param[in] other The underlying Error to nest.
   */
  explicit InternalError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InternalError";
};

/// @brief Specific error for when a file cannot be opened.
class FileOpenError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] filename The path to the file which could not be opened.
   */
  explicit FileOpenError(std::string_view filename);

  /**
   * @brief Nested error constructor.
   * @param[in] filename The path to the file which could not be opened.
   * @param[in] other The error to nest.
   */
  explicit FileOpenError(std::string_view filename, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "FileOpenError";
};

/// @brief Specific error for when an operation failed due to incompatible user input.
class InvalidInputError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The error message detailing the invalid input.
   */
  explicit InvalidInputError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The error message detailing the invalid input.
   * @param[in] other The error to nest.
   */
  explicit InvalidInputError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidInputError";
};

/// @brief An operation which is not permitted according to the current device status was attempted.
class OperationRefusedError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit OperationRefusedError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit OperationRefusedError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "OperationRefusedError";
};

/// @brief Error reported by the gRPC transport library.
class GrpcLibError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit GrpcLibError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit GrpcLibError(std::string_view message, const Error& other);

  /**
   * @brief Error constructor including gRPC error codes.
   * @param[in] code The error code returned by the gRPC library.
   * @param[in] message The human-readable error message.
   */
  explicit GrpcLibError(std::uint64_t code, std::string_view message);

  /**
   * @brief Nested error constructor including gRPC error codes.
   * @param[in] code The error code returned by the gRPC library.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit GrpcLibError(std::uint64_t code, std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "GrpcLibError";
};

/// @brief Error reported by the Astarte message hub library.
class MsgHubError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit MsgHubError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit MsgHubError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "MsgHubError";
};

// -----------------------------------------------------------------------------
// Interface validation errors
// -----------------------------------------------------------------------------

/// @brief Error during the Interface validation process.
class InterfaceValidationError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit InterfaceValidationError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit InterfaceValidationError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InterfaceValidationError";
};

/// @brief Error indicating that the minor or major version of an interface is incorrect.
class InvalidInterfaceVersionError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit InvalidInterfaceVersionError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit InvalidInterfaceVersionError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidInterfaceVersionError";
};

/// @brief Error indicating that the provided interface type is incorrect.
class InvalidInterfaceTypeError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit InvalidInterfaceTypeError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit InvalidInterfaceTypeError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidInterfaceTypeError";
};

/// @brief Error indicating that the provided interface ownership is incorrect.
class InvalidInterfaceOwnershipeError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit InvalidInterfaceOwnershipeError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit InvalidInterfaceOwnershipeError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidInterfaceOwnershipeError";
};

/// @brief Error indicating that the provided interface aggregation is incorrect.
class InvalidInterfaceAggregationError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit InvalidInterfaceAggregationError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit InvalidInterfaceAggregationError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidInterfaceAggregationError";
};

/// @brief Error indicating that the provided Astarte type is incorrect.
class InvalidAstarteTypeError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit InvalidAstarteTypeError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit InvalidAstarteTypeError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidAstarteTypeError";
};

/// @brief Error indicating that the provided Astarte reliability is incorrect.
class InvalidReliabilityError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit InvalidReliabilityError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit InvalidReliabilityError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidReliabilityError";
};

/// @brief Error indicating that the provided Astarte retention is incorrect.
class InvalidRetentionError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit InvalidRetentionError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit InvalidRetentionError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidRetentionError";
};

/// @brief Error indicating that the provided Astarte database retention policy is incorrect.
class InvalidDatabaseRetentionPolicyError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param[in] message The human-readable error message.
   */
  explicit InvalidDatabaseRetentionPolicyError(std::string_view message);

  /**
   * @brief Nested error constructor.
   * @param[in] message The human-readable error message.
   * @param[in] other The error to nest.
   */
  explicit InvalidDatabaseRetentionPolicyError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidDatabaseRetentionPolicyError";
};

}  // namespace astarte::device

#if !defined(ASTARTE_TRANSPORT_GRPC)
// We accept this circular inclusion as it's required for the forward declarations above to work
// even when an user includes only this header.
// NOLINTNEXTLINE(misc-header-include-cycle)
#include "astarte_device_sdk/mqtt/errors.hpp"
#endif

/// @brief Formatter specialization for astarte::device::Error.
template <>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct astarte_fmt::formatter<astarte::device::Error> {
  /**
   * @brief Parses the format string.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the Error object by visiting the variant.
   * @param[in] err_variant The Error variant instance to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output context.
   */
  template <typename FormatContext>
  auto format(const astarte::device::Error& err_variant, FormatContext& ctx) const {
    return std::visit(
        [&ctx](const auto& err) {
          const auto& base_err = static_cast<const astarte::device::ErrorBase&>(err);
          return astarte_fmt::format_to(ctx.out(), "{}", base_err);
        },
        err_variant);
  }
};

/// @brief Formatter specialization for astarte::device::ErrorBase.
template <>
struct astarte_fmt::formatter<astarte::device::ErrorBase> {
  /**
   * @brief Parses the format string.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the ErrorBase object including any nested errors.
   * @param[in] err The ErrorBase instance to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output context.
   */
  template <typename FormatContext>
  auto format(const astarte::device::ErrorBase& err, FormatContext& ctx) const {
    auto out = astarte_fmt::format_to(ctx.out(), "{}: {}", err.type(), err.message());

    std::string indent;
    const astarte::device::ErrorBase* current = &err;
    while (const auto& nested = current->nested_error()) {
      indent += "  ";
      out = astarte_fmt::format_to(out, "\n{}-> {}: {}", indent, nested->type(), nested->message());
      current = nested.get();
    }

    return out;
  }
};

#endif  // ASTARTE_DEVICE_SDK_ERRORS_H
