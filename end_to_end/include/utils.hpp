// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <algorithm>
#include <list>
#include <sstream>
#include <string>

/**
 * @brief Convert a time_point to a UTC string compatible with Astarte.
 * @param timestamp Reference to the output iterator where the result is written.
 * @return The UTC time as a string in the format "YYYY-MM-DDTHH:MM:SS.sssZ"
 */
auto time_point_to_utc(const std::chrono::system_clock::time_point* timestamp) -> std::string {
  auto ms_since_epoch =
      std::chrono::duration_cast<std::chrono::milliseconds>(timestamp->time_since_epoch());
  auto s_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(ms_since_epoch);
  long long milliseconds = (ms_since_epoch - s_since_epoch).count();

  std::time_t c_time = std::chrono::system_clock::to_time_t(*timestamp);

  // get the UTC time structure
  std::tm* tm_gmt = std::gmtime(&c_time);
  if (!tm_gmt) {
    return "Error: Failed to convert to UTC time.";
  }

  std::ostringstream oss;
  oss << std::put_time(tm_gmt, "%Y-%m-%dT%H:%M:%S")                // format YYYY-MM-DDTHH:MM:SS
      << "." << std::setw(3) << std::setfill('0') << milliseconds  // format .sss
      << "Z";                                                      // append the 'Z' for UTC

  return oss.str();
}

template <typename T>
auto format_list(const std::list<T>& input) -> std::string {
  std::ostringstream oss;
  oss << "{";
  for (auto v : input) {
    oss << v << ", ";
  }
  oss << "}";
  return oss.str();
}

template <typename T>
bool compare_lists(const std::list<T>& list1, const std::list<T>& list2) {
  if (list1.size() != list2.size()) {
    return false;
  }

  return std::is_permutation(list1.begin(), list1.end(), list2.begin());
}
