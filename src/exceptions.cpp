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
    : AstarteException("Failed to open file: " + std::string(filename)), filename_(filename) {}
auto AstarteFileOpenException::get_filename() const -> const std::string& { return filename_; }

AstarteInvalidInputException::AstarteInvalidInputException(std::string_view err_message)
    : AstarteException(err_message) {}

AstarteInternalException::AstarteInternalException(std::string_view err_message)
    : AstarteException(err_message) {}

AstarteOperationRefusedException::AstarteOperationRefusedException(std::string_view err_message)
    : AstarteException(err_message) {}

}  // namespace AstarteDeviceSdk
