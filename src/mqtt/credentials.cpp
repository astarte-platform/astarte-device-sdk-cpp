// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"

namespace config {

namespace astarte_tl = AstarteDeviceSdk::astarte_tl;

using AstarteDeviceSdk::AstarteError;
using AstarteDeviceSdk::AstarteReadCredentialError;
using AstarteDeviceSdk::AstarteWriteCredentialError;

auto read_from_file(const std::filesystem::path& file_path)
    -> astarte_tl::expected<std::string, AstarteError> {
  std::ifstream interface_file(file_path, std::ios::in);
  if (!interface_file.is_open()) {
    return astarte_tl::unexpected(AstarteReadCredentialError(
        astarte_fmt::format("Could not open the credential file: {}", file_path.string())));
  }

  // read the entire file content into a string
  std::string data((std::istreambuf_iterator<char>(interface_file)),
                   std::istreambuf_iterator<char>());
  interface_file.close();

  return data;
}

auto write_to_file(const std::filesystem::path& file_path, std::string_view data)
    -> astarte_tl::expected<void, AstarteError> {
  // open an output file stream (ofstream) using the path object
  // the file is automatically closed when 'output_file' goes out of scope
  std::ofstream output_file(file_path);

  // error if the file was not opened successfully
  if (!output_file.is_open()) {
    return astarte_tl::unexpected(AstarteWriteCredentialError(
        astarte_fmt::format("couldn't open file {}", file_path.string())));
  }

  output_file << data;

  return {};
}

// helper function to reduce complexity of secure_shred_file [readability-function-size]
auto overwrite_file_zeros(FILE* file, int64_t size) -> astarte_tl::expected<void, AstarteError> {
  if (std::fseek(file, 0, SEEK_SET) != 0) {
    return astarte_tl::unexpected(AstarteWriteCredentialError(astarte_fmt::format(
        "failed to rewind file, {}", std::error_code(errno, std::generic_category()).message())));
  }

  constexpr size_t kBufSize = 4096;
  std::vector<unsigned char> buf(kBufSize, 0);
  int64_t written = 0;

  while (written < size) {
    const int64_t remaining = size - written;
    const size_t to_write =
        (remaining > static_cast<int64_t>(kBufSize)) ? kBufSize : static_cast<size_t>(remaining);

    const size_t write_count = std::fwrite(buf.data(), 1, to_write, file);

    if (write_count != to_write) {
      return astarte_tl::unexpected(AstarteWriteCredentialError(
          astarte_fmt::format("failed to write zeros to file, {}",
                              std::error_code(errno, std::generic_category()).message())));
    }
    written += static_cast<int64_t>(write_count);
  }
  return {};
}

auto secure_shred_file(const std::string& path) -> astarte_tl::expected<void, AstarteError> {
  FILE* file = std::fopen(path.c_str(), "rb+");

  if (file == nullptr) {
    return astarte_tl::unexpected(AstarteWriteCredentialError(astarte_fmt::format(
        "failed to open the file, {}", std::error_code(errno, std::generic_category()).message())));
  }

  if (std::fseek(file, 0, SEEK_END) != 0) {
    (void)std::fclose(file);
    return astarte_tl::unexpected(AstarteWriteCredentialError(
        astarte_fmt::format("failed to seek end of file, {}",
                            std::error_code(errno, std::generic_category()).message())));
  }

  const int64_t size = std::ftell(file);
  if (size == -1) {
    (void)std::fclose(file);
    return astarte_tl::unexpected(AstarteWriteCredentialError(
        astarte_fmt::format("failed to tell file size, {}",
                            std::error_code(errno, std::generic_category()).message())));
  }

  auto overwrite_result = overwrite_file_zeros(file, size);
  if (!overwrite_result) {
    (void)std::fclose(file);
    return overwrite_result;
  }

  if (std::fflush(file) != 0) {
    (void)std::fclose(file);
    return astarte_tl::unexpected(AstarteWriteCredentialError(astarte_fmt::format(
        "failed to flush file, {}", std::error_code(errno, std::generic_category()).message())));
  }

  if (fsync(fileno(file)) != 0) {
    (void)std::fclose(file);
    return astarte_tl::unexpected(AstarteWriteCredentialError(
        astarte_fmt::format("failed to flush and sync modifications, {}",
                            std::error_code(errno, std::generic_category()).message())));
  }

  if (std::fclose(file) != 0) {
    return astarte_tl::unexpected(AstarteWriteCredentialError(astarte_fmt::format(
        "failed to close file, {}", std::error_code(errno, std::generic_category()).message())));
  }

  // delete the file
  std::error_code error_code;
  std::filesystem::remove(path, error_code);
  if (error_code) {
    return astarte_tl::unexpected(AstarteWriteCredentialError(
        astarte_fmt::format("failed to delete the file, {}", error_code.message())));
  }

  return {};
}

}  // namespace config
