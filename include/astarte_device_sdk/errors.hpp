// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_ERRORS_H
#define ASTARTE_DEVICE_SDK_ERRORS_H

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
#endif

/**
 * @brief A variant type representing any possible error from the Astarte device library.
 *
 * This type is intended to be used as the error type 'E' in std::expected<T, E>.
 */
using Error =
    std::variant<DataSerializationError, InternalError, FileOpenError, InvalidInputError,
                 InterfaceValidationError, InvalidInterfaceVersionError, InvalidInterfaceTypeError,
                 InvalidInterfaceOwnershipeError, InvalidInterfaceAggregationError,
                 InvalidAstarteTypeError, InvalidReliabilityError, InvalidRetentionError,
                 InvalidDatabaseRetentionPolicyError,
#if !defined(ASTARTE_TRANSPORT_GRPC)
                 OperationRefusedError, GrpcLibError, MsgHubError, JsonParsingError,
                 DeviceRegistrationError, PairingApiError, MqttError, InvalidUrlError,
                 RetrieveBrokerUrlError, ReadCredentialError, WriteCredentialError,
                 PairingConfigError, CryptoError, UuidError, HttpError, MqttConnectionError>;
#else
                 OperationRefusedError, GrpcLibError, MsgHubError>;
#endif

/**
 * @brief Base error class representing any possible error from the Astarte device library.
 *
 * This type is intended to be used as the error type 'E' in std::expected<T, E>.
 */
class ErrorBase {
 public:
  /** @brief Destructor for the Astarte error. */
  virtual ~ErrorBase() = default;
  /** @brief Default copy constructor. */
  ErrorBase(const ErrorBase&) = default;
  /**
   * @brief Default copy assignment operator.
   * @return The new class.
   */
  auto operator=(const ErrorBase&) -> ErrorBase& = default;
  /** @brief Delete move constructor. */
  ErrorBase(ErrorBase&&) = delete;
  /**
   * @brief Default move assignment operator.
   * @return The new instance.
   */
  auto operator=(ErrorBase&&) -> ErrorBase& = default;

  /**
   * @brief Return the message encapsulated in the error.
   * @return The error message.
   */
  [[nodiscard]] auto message() const -> const std::string&;
  /**
   * @brief Return the type encapsulated in the error.
   * @return The error type.
   */
  [[nodiscard]] auto type() const -> const std::string&;
  /**
   * @brief Return the nested error.
   * @return The error.
   */
  [[nodiscard]] auto nested_error() const -> const std::shared_ptr<ErrorBase>&;

 protected:
  /**
   * @brief Constructor for the Astarte error.
   * @param type The name of the error type.
   * @param message The error message.
   */
  explicit ErrorBase(std::string_view type, std::string_view message);
  /**
   * @brief Wrapping constructor for the Astarte error.
   * @param type The name of the error type.
   * @param message The error message.
   * @param other An error to encapsulate within this base error.
   */
  explicit ErrorBase(std::string_view type, std::string_view message, const ErrorBase& other);

 private:
  std::string type_;
  std::string message_;
  std::shared_ptr<ErrorBase> other_;
};

/**
 * @brief Specific error for when a serializaion operation failed.
 */
class DataSerializationError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit DataSerializationError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit DataSerializationError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "DataSerializationError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class InternalError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InternalError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InternalError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InternalError";
};

/**
 * @brief Specific error for when a file cannot be opened.
 */
class FileOpenError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param filename The file which could not be opened.
   */
  explicit FileOpenError(std::string_view filename);
  /**
   * @brief Nested error constructor.
   * @param filename The file which could not be opened.
   * @param other The error to nest.
   */
  explicit FileOpenError(std::string_view filename, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "FileOpenError";
};

/**
 * @brief Specific error for when an operation failed due to incompatible user input.
 */
class InvalidInputError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InvalidInputError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InvalidInputError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidInputError";
};

/**
 * @brief Attempted an operation which is not permitted according to the current device status.
 */
class OperationRefusedError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit OperationRefusedError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit OperationRefusedError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "OperationRefusedError";
};

/**
 * @brief Error reported by the gRPC transport library.
 */
class GrpcLibError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit GrpcLibError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit GrpcLibError(std::string_view message, const Error& other);
  /**
   * @brief Error constructor including gRPC error codes.
   * @param code The error code returned by the gRPC library.
   * @param message The error message.
   */
  explicit GrpcLibError(std::uint64_t code, std::string_view message);
  /**
   * @brief Nested error constructor including gRPC error codes.
   * @param code The error code returned by the gRPC library.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit GrpcLibError(std::uint64_t code, std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "GrpcLibError";
};

/**
 * @brief Error reported by the Astarte message hub library.
 */
class MsgHubError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit MsgHubError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit MsgHubError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "MsgHubError";
};

/************************************************
 *       Interface validation errors       *
 ***********************************************/

/**
 * @brief Error during the Interface validation.
 */
class InterfaceValidationError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InterfaceValidationError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InterfaceValidationError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InterfaceValidationError";
};

/**
 * @brief Either the minor or the major version is incorrect.
 */
class InvalidInterfaceVersionError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InvalidInterfaceVersionError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InvalidInterfaceVersionError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidInterfaceVersionError";
};

/**
 * @brief The provided interface type is incorrect.
 */
class InvalidInterfaceTypeError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InvalidInterfaceTypeError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InvalidInterfaceTypeError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidInterfaceTypeError";
};

/**
 * @brief The provided interface ownership is incorrect.
 */
class InvalidInterfaceOwnershipeError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InvalidInterfaceOwnershipeError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InvalidInterfaceOwnershipeError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidInterfaceOwnershipeError";
};

/**
 * @brief The provided interface aggregation is incorrect.
 */
class InvalidInterfaceAggregationError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InvalidInterfaceAggregationError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InvalidInterfaceAggregationError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidInterfaceAggregationError";
};

/**
 * @brief The provided Astarte type is incorrect.
 */
class InvalidAstarteTypeError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InvalidAstarteTypeError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InvalidAstarteTypeError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidAstarteTypeError";
};

/**
 * @brief The provided Astarte reliability is incorrect.
 */
class InvalidReliabilityError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InvalidReliabilityError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InvalidReliabilityError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidReliabilityError";
};

/**
 * @brief The provided Astarte retention is incorrect.
 */
class InvalidRetentionError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InvalidRetentionError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InvalidRetentionError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidRetentionError";
};

/**
 * @brief The provided Astarte database retention policy is incorrect.
 */
class InvalidDatabaseRetentionPolicyError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InvalidDatabaseRetentionPolicyError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
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

/**
 * @brief Formatter specialization for astarte::device::Error.
 */
template <>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct astarte_fmt::formatter<astarte::device::Error> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the Error object.
   * @param err_variant The Error to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
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

/**
 * @brief Formatter specialization for astarte::device::ErrorBase.
 */
template <>
struct astarte_fmt::formatter<astarte::device::ErrorBase> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the ErrorBase object.
   * @param err The ErrorBase to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
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
