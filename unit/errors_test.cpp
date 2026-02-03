// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/errors.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "astarte_device_sdk/formatter.hpp"

using astarte::device::AstarteError;
using astarte::device::AstarteFileOpenError;
using astarte::device::AstarteGrpcLibError;
using astarte::device::AstarteInternalError;

TEST(AstarteTestErrors, Nesting) {
  AstarteError file_open{AstarteFileOpenError{"file name"}};
  AstarteError internal{AstarteInternalError{"Invalid input or something else", file_open}};
  std::string formatted = astarte_fmt::format("{}", internal);
  std::string expected = R"(AstarteInternalError: Invalid input or something else
  -> AstarteFileOpenError: file name)";
  ASSERT_EQ(expected, formatted);
}

TEST(AstarteTestErrors, Grpc) {
  std::uint64_t code = 12;
  std::string_view message = "A simple error message";
  AstarteError grpc_err{AstarteGrpcLibError{code, message}};
  std::string formatted = astarte_fmt::format("{}", grpc_err);
  std::string expected = R"(AstarteGrpcLibError: code(12)-message(A simple error message))";
  ASSERT_EQ(expected, formatted);
}
