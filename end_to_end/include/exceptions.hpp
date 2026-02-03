// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <exception>
#include <string>

class EndToEndException : public std::exception {
 public:
  explicit EndToEndException(std::string message) : message_(std::move(message)) {}
  [[nodiscard]] auto what() const noexcept -> const char* override { return message_.c_str(); }

 private:
  std::string message_;
};

class EndToEndConfigException : public EndToEndException {
 public:
  explicit EndToEndConfigException(const std::string& err_message)
      : EndToEndException(err_message) {}
};

class EndToEndAstarteDeviceException : public EndToEndException {
 public:
  explicit EndToEndAstarteDeviceException(const std::string& err_message)
      : EndToEndException(err_message) {}
};

class EndToEndHTTPException : public EndToEndException {
 public:
  explicit EndToEndHTTPException(const std::string& err_message) : EndToEndException(err_message) {}
};

class EndToEndTimeoutException : public EndToEndException {
 public:
  explicit EndToEndTimeoutException(const std::string& err_message)
      : EndToEndException(err_message) {}
};

class EndToEndMismatchException : public EndToEndException {
 public:
  explicit EndToEndMismatchException(const std::string& err_message)
      : EndToEndException(err_message) {}
};
