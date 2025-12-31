// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <unistd.h>

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

auto secure_shred_file(const std::string& path) -> astarte_tl::expected<void, AstarteError> {
  FILE* file = std::fopen(path.c_str(), "rb+");
  if (!file) {
    // capture the global 'errno', wrap it in an error_code, and return as unexpected
    return astarte_tl::unexpected(AstarteWriteCredentialError(astarte_fmt::format(
        "failed to open the file, {}", std::error_code(errno, std::generic_category()).message())));
  }

  // get size
  std::fseek(file, 0, SEEK_END);
  long size = std::ftell(file);
  std::rewind(file);

  // write zeros
  const size_t buf_size = 4096;
  std::vector<unsigned char> buf(buf_size, 0);
  long written = 0;

  while (written < size) {
    long to_write = (size - written > buf_size) ? buf_size : (size - written);
    size_t write_count = std::fwrite(buf.data(), 1, to_write, file);

    // check if write failed
    if (write_count != to_write) {
      std::fclose(file);
      return astarte_tl::unexpected(AstarteWriteCredentialError(
          astarte_fmt::format("failed to write zeros to file, {}",
                              std::error_code(errno, std::generic_category()).message())));
    }
    written += to_write;
  }

  // flush and sync
  std::fflush(file);
  if (fsync(fileno(file)) != 0) {
    std::fclose(file);
    return astarte_tl::unexpected(AstarteWriteCredentialError(
        astarte_fmt::format("failed to flush and sync modifications, {}",
                            std::error_code(errno, std::generic_category()).message())));
  }

  std::fclose(file);

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
