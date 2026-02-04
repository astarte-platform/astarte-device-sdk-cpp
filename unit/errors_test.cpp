// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/errors.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "astarte_device_sdk/formatter.hpp"

using astarte::device::Error;
using astarte::device::FileOpenError;
using astarte::device::GrpcLibError;
using astarte::device::InternalError;

TEST(AstarteTestErrors, Nesting) {
  Error file_open{FileOpenError{"file name"}};
  Error internal{InternalError{"Invalid input or something else", file_open}};
  std::string formatted = astarte_fmt::format("{}", internal);
  std::string expected = R"(InternalError: Invalid input or something else
  -> FileOpenError: file name)";
  ASSERT_EQ(expected, formatted);
}

TEST(AstarteTestErrors, Grpc) {
  std::uint64_t code = 12;
  std::string_view message = "A simple error message";
  Error grpc_err{GrpcLibError{code, message}};
  std::string formatted = astarte_fmt::format("{}", grpc_err);
  std::string expected = R"(GrpcLibError: code(12)-message(A simple error message))";
  ASSERT_EQ(expected, formatted);
}
