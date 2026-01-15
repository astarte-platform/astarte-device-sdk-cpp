// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

namespace constants::time {

using namespace std::chrono_literals;

constexpr std::chrono::system_clock::time_point TIMESTAMP =
    std::chrono::sys_days{1987y / 5 / 1} + 15h + 32min;

}  // namespace constants::time
