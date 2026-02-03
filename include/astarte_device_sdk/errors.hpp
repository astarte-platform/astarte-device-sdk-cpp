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

class AstarteDataSerializationError;
class AstarteFileOpenError;
class AstarteInvalidInputError;
class AstarteInternalError;
class AstarteOperationRefusedError;
class AstarteGrpcLibError;
class AstarteMsgHubError;
class AstarteInterfaceValidationError;
class AstarteInvalidInterfaceVersionError;
class AstarteInvalidInterfaceTypeError;
class AstarteInvalidInterfaceOwnershipeError;
class AstarteInvalidInterfaceAggregationError;
class AstarteInvalidReliabilityError;
class AstarteInvalidAstarteTypeError;
class AstarteInvalidRetentionError;
class AstarteInvalidDatabaseRetentionPolicyError;
#if !defined(ASTARTE_TRANSPORT_GRPC)
class AstarteJsonParsingError;
class AstarteDeviceRegistrationError;
class AstartePairingApiError;
class AstarteMqttError;
class AstarteInvalidUrlError;
class AstarteRetrieveBrokerUrlError;
class AstarteReadCredentialError;
class AstarteWriteCredentialError;
class AstartePairingConfigError;
class AstarteCryptoError;
class AstarteUuidError;
class AstarteHttpError;
class AstarteMqttConnectionError;
#endif

/**
 * @brief A variant type representing any possible error from the Astarte device library.
 *
 * This type is intended to be used as the error type 'E' in std::expected<T, E>.
 */
using AstarteError =
    std::variant<AstarteDataSerializationError, AstarteInternalError, AstarteFileOpenError,
                 AstarteInvalidInputError, AstarteInterfaceValidationError,
                 AstarteInvalidInterfaceVersionError, AstarteInvalidInterfaceTypeError,
                 AstarteInvalidInterfaceOwnershipeError, AstarteInvalidInterfaceAggregationError,
                 AstarteInvalidAstarteTypeError, AstarteInvalidReliabilityError,
                 AstarteInvalidRetentionError, AstarteInvalidDatabaseRetentionPolicyError,
#if !defined(ASTARTE_TRANSPORT_GRPC)
                 AstarteOperationRefusedError, AstarteGrpcLibError, AstarteMsgHubError,
                 AstarteJsonParsingError, AstarteDeviceRegistrationError, AstartePairingApiError,
                 AstarteMqttError, AstarteInvalidUrlError, AstarteRetrieveBrokerUrlError,
                 AstarteReadCredentialError, AstarteWriteCredentialError, AstartePairingConfigError,
                 AstarteCryptoError, AstarteUuidError, AstarteHttpError,
                 AstarteMqttConnectionError>;
#else
                 AstarteOperationRefusedError, AstarteGrpcLibError, AstarteMsgHubError>;
#endif

/**
 * @brief Base error class representing any possible error from the Astarte device library.
 *
 * This type is intended to be used as the error type 'E' in std::expected<T, E>.
 */
class AstarteErrorBase {
 public:
  /** @brief Destructor for the Astarte error. */
  virtual ~AstarteErrorBase() = default;
  /** @brief Default copy constructor. */
  AstarteErrorBase(const AstarteErrorBase&) = default;
  /**
   * @brief Default copy assignment operator.
   * @return The new class.
   */
  auto operator=(const AstarteErrorBase&) -> AstarteErrorBase& = default;
  /** @brief Delete move constructor. */
  AstarteErrorBase(AstarteErrorBase&&) = delete;
  /**
   * @brief Default move assignment operator.
   * @return The new instance.
   */
  auto operator=(AstarteErrorBase&&) -> AstarteErrorBase& = default;

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
  [[nodiscard]] auto nested_error() const -> const std::shared_ptr<AstarteErrorBase>&;

 protected:
  /**
   * @brief Constructor for the Astarte error.
   * @param type The name of the error type.
   * @param message The error message.
   */
  explicit AstarteErrorBase(std::string_view type, std::string_view message);
  /**
   * @brief Wrapping constructor for the Astarte error.
   * @param type The name of the error type.
   * @param message The error message.
   * @param other An error to encapsulate within this base error.
   */
  explicit AstarteErrorBase(std::string_view type, std::string_view message,
                            const AstarteErrorBase& other);

 private:
  std::string type_;
  std::string message_;
  std::shared_ptr<AstarteErrorBase> other_;
};

/**
 * @brief Specific error for when a serializaion operation failed.
 */
class AstarteDataSerializationError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteDataSerializationError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteDataSerializationError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteDataSerializationError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class AstarteInternalError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInternalError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInternalError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInternalError";
};

/**
 * @brief Specific error for when a file cannot be opened.
 */
class AstarteFileOpenError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param filename The file which could not be opened.
   */
  explicit AstarteFileOpenError(std::string_view filename);
  /**
   * @brief Nested error constructor.
   * @param filename The file which could not be opened.
   * @param other The error to nest.
   */
  explicit AstarteFileOpenError(std::string_view filename, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteFileOpenError";
};

/**
 * @brief Specific error for when an operation failed due to incompatible user input.
 */
class AstarteInvalidInputError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidInputError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidInputError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidInputError";
};

/**
 * @brief Attempted an operation which is not permitted according to the current device status.
 */
class AstarteOperationRefusedError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteOperationRefusedError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteOperationRefusedError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteOperationRefusedError";
};

/**
 * @brief Error reported by the gRPC transport library.
 */
class AstarteGrpcLibError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteGrpcLibError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteGrpcLibError(std::string_view message, const AstarteError& other);
  /**
   * @brief Error constructor including gRPC error codes.
   * @param code The error code returned by the gRPC library.
   * @param message The error message.
   */
  explicit AstarteGrpcLibError(std::uint64_t code, std::string_view message);
  /**
   * @brief Nested error constructor including gRPC error codes.
   * @param code The error code returned by the gRPC library.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteGrpcLibError(std::uint64_t code, std::string_view message,
                               const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteGrpcLibError";
};

/**
 * @brief Error reported by the Astarte message hub library.
 */
class AstarteMsgHubError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteMsgHubError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteMsgHubError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteMsgHubError";
};

/************************************************
 *       Interface validation errors       *
 ***********************************************/

/**
 * @brief Error during the Interface validation.
 */
class AstarteInterfaceValidationError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInterfaceValidationError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInterfaceValidationError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInterfaceValidationError";
};

/**
 * @brief Either the minor or the major version is incorrect.
 */
class AstarteInvalidInterfaceVersionError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidInterfaceVersionError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidInterfaceVersionError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidInterfaceVersionError";
};

/**
 * @brief The provided interface type is incorrect.
 */
class AstarteInvalidInterfaceTypeError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidInterfaceTypeError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidInterfaceTypeError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidInterfaceTypeError";
};

/**
 * @brief The provided interface ownership is incorrect.
 */
class AstarteInvalidInterfaceOwnershipeError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidInterfaceOwnershipeError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidInterfaceOwnershipeError(std::string_view message,
                                                  const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidInterfaceOwnershipeError";
};

/**
 * @brief The provided interface aggregation is incorrect.
 */
class AstarteInvalidInterfaceAggregationError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidInterfaceAggregationError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidInterfaceAggregationError(std::string_view message,
                                                   const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidInterfaceAggregationError";
};

/**
 * @brief The provided Astarte type is incorrect.
 */
class AstarteInvalidAstarteTypeError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidAstarteTypeError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidAstarteTypeError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidAstarteTypeError";
};

/**
 * @brief The provided Astarte reliability is incorrect.
 */
class AstarteInvalidReliabilityError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidReliabilityError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidReliabilityError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidReliabilityError";
};

/**
 * @brief The provided Astarte retention is incorrect.
 */
class AstarteInvalidRetentionError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidRetentionError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidRetentionError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidRetentionError";
};

/**
 * @brief The provided Astarte database retention policy is incorrect.
 */
class AstarteInvalidDatabaseRetentionPolicyError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidDatabaseRetentionPolicyError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidDatabaseRetentionPolicyError(std::string_view message,
                                                      const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidDatabaseRetentionPolicyError";
};

}  // namespace astarte::device

#if !defined(ASTARTE_TRANSPORT_GRPC)
// We accept this circular inclusion as it's required for the forward declarations above to work
// even when an user includes only this header.
// NOLINTNEXTLINE(misc-header-include-cycle)
#include "astarte_device_sdk/mqtt/errors.hpp"
#endif

/**
 * @brief Formatter specialization for astarte::device::AstarteError.
 */
template <>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct astarte_fmt::formatter<astarte::device::AstarteError> {
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
   * @brief Format the AstarteError object.
   * @param err_variant The AstarteError to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::AstarteError& err_variant, FormatContext& ctx) const {
    return std::visit(
        [&ctx](const auto& err) {
          const auto& base_err = static_cast<const astarte::device::AstarteErrorBase&>(err);
          return astarte_fmt::format_to(ctx.out(), "{}", base_err);
        },
        err_variant);
  }
};

/**
 * @brief Formatter specialization for astarte::device::AstarteErrorBase.
 */
template <>
struct astarte_fmt::formatter<astarte::device::AstarteErrorBase> {
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
   * @brief Format the AstarteErrorBase object.
   * @param err The AstarteErrorBase to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::AstarteErrorBase& err, FormatContext& ctx) const {
    auto out = astarte_fmt::format_to(ctx.out(), "{}: {}", err.type(), err.message());

    std::string indent;
    const astarte::device::AstarteErrorBase* current = &err;
    while (const auto& nested = current->nested_error()) {
      indent += "  ";
      out = astarte_fmt::format_to(out, "\n{}-> {}: {}", indent, nested->type(), nested->message());
      current = nested.get();
    }

    return out;
  }
};

#endif  // ASTARTE_DEVICE_SDK_ERRORS_H
