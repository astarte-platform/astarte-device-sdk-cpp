// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_ERRORS_H
#define ASTARTE_DEVICE_SDK_ERRORS_H

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#if defined(ASTARTE_USE_TL_EXPECTED)
#include "tl/expected.hpp"
#else
#include <expected>
#endif

namespace AstarteDeviceSdk {

#if defined(ASTARTE_USE_TL_EXPECTED)
namespace astarte_tl = ::tl;
#else
namespace astarte_tl = ::std;
#endif

class AstarteFileOpenError;
class AstarteInvalidInputError;
class AstarteInternalError;
class AstarteOperationRefusedError;
class AstarteGrpcLibError;
class AstarteMsgHubError;
class AstarteInvalidVersionError;
class AstarteInvalidInterfaceTypeError;
class AstarteInvalidInterfaceOwnershipeError;
class AstarteInvalidAggregationError;
class AstarteInvalidAstarteTypeError;
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
    std::variant<AstarteInternalError, AstarteFileOpenError, AstarteInvalidInputError,
                 AstarteInvalidVersionError, AstarteInvalidInterfaceTypeError,
                 AstarteInvalidInterfaceOwnershipeError, AstarteInvalidAggregationError,
                 AstarteInvalidAstarteTypeError,
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
  virtual ~AstarteErrorBase();
  /**
   * @brief Return the message encapsulated in the error.
   * @return The error message.
   */
  auto message() const -> const std::string&;
  /**
   * @brief Return the type encapsulated in the error.
   * @return The error type.
   */
  auto type() const -> const std::string&;
  /**
   * @brief Return the nested error.
   * @return The error.
   */
  auto nested_error() const -> const std::shared_ptr<AstarteErrorBase>&;

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
 * @brief Either the minor or the major version is incorrect.
 */
class AstarteInvalidVersionError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidVersionError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidVersionError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidVersionError";
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
class AstarteInvalidAggregationError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidAggregationError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidAggregationError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidAggregationError";
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

}  // namespace AstarteDeviceSdk

#if !defined(ASTARTE_TRANSPORT_GRPC)
// We accept this circular inclusion as it's required for the forward declarations above to work
// even when an user includes only this header.
// NOLINTNEXTLINE(misc-header-include-cycle)
#include "astarte_device_sdk/mqtt/errors.hpp"
#endif

#endif  // ASTARTE_DEVICE_SDK_ERRORS_H
