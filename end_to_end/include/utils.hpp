// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <algorithm>
#include <list>
#include <sstream>
#include <string>

template <typename T>
auto format_list(const std::list<T>& input) -> std::string {
  std::ostringstream oss;
  oss << "{";
  for (auto v : input) {
    oss << v.format() << ", ";
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
