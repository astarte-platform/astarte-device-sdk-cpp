// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/exceptions.h"

#include <string>
#include <utility>

namespace AstarteDeviceSdk {

AstarteException::AstarteException(std::string message) : message_(std::move(message)) {}
auto AstarteException::what() const noexcept -> const char* { return message_.c_str(); }

AstarteFileOpenException::AstarteFileOpenException(const std::string& filename)
    : AstarteException("Failed to open file: " + filename), filename_(filename) {}
auto AstarteFileOpenException::get_filename() const -> const std::string& { return filename_; }

AstarteIncompatibleInputException::AstarteIncompatibleInputException(const std::string& err_message)
    : AstarteException(err_message) {}

}  // namespace AstarteDeviceSdk
