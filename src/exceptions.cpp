// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/exceptions.hpp"

#include <string>
#include <string_view>

namespace AstarteDeviceSdk {

AstarteException::AstarteException(std::string_view message) : message_(message) {}
auto AstarteException::what() const noexcept -> const char* { return message_.c_str(); }

AstarteFileOpenException::AstarteFileOpenException(std::string_view filename)
    : AstarteException("AstarteFileOpenException(" + std::string(filename) + ")"),
      filename_(filename) {}
auto AstarteFileOpenException::get_filename() const -> const std::string& { return filename_; }

AstarteInvalidInputException::AstarteInvalidInputException(std::string_view err_message)
    : AstarteException("AstarteInvalidInputException(" + std::string(err_message) + ")") {}

AstarteInternalException::AstarteInternalException(std::string_view err_message)
    : AstarteException("AstarteInternalException(" + std::string(err_message) + ")") {}

AstarteOperationRefusedException::AstarteOperationRefusedException(std::string_view err_message)
    : AstarteException("AstarteOperationRefusedException(" + std::string(err_message) + ")") {}

/************************************************
 *       Interface validation exceptions       *
 ***********************************************/

InterfaceValidationException::InterfaceValidationException(const std::string& err_message)
    : AstarteException("InterfaceValidationException(" + err_message + ")") {}

InvalidVersionException::InvalidVersionException(const std::string& err_message)
    : InterfaceValidationException("InvalidVersionException(" + err_message + ")") {}

InvalidInterfaceTypeException::InvalidInterfaceTypeException(const std::string& err_message)
    : InterfaceValidationException("InvalidInterfaceTypeException(" + err_message + ")") {}

InvalidInterfaceOwnershipeException::InvalidInterfaceOwnershipeException(
    const std::string& err_message)
    : InterfaceValidationException("InvalidInterfaceOwnershipeException(" + err_message + ")") {}

InvalidAggregationException::InvalidAggregationException(const std::string& err_message)
    : InterfaceValidationException("InvalidAggregationException(" + err_message + ")") {}

InvalidAstarteTypeException::InvalidAstarteTypeException(const std::string& err_message)
    : InterfaceValidationException("InvalidAstarteTypeException(" + err_message + ")") {}

}  // namespace AstarteDeviceSdk
