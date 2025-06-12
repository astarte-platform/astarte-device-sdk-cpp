// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef EXPONENTIAL_BACKOFF_H
#define EXPONENTIAL_BACKOFF_H

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <random>

namespace AstarteDeviceSdk {

class ExponentialBackoff {
 public:
  /**
   * @brief Constructs an ExponentialBackoff instance.
   * @param initial_delay The value for the first backoff delay.
   * @param max_delay The upper bound for all the backoff delays.
   */
  ExponentialBackoff(std::chrono::milliseconds initial_delay, std::chrono::milliseconds max_delay)
      : initial_delay_(initial_delay), max_delay_(max_delay) {}

  /**
   * @brief Calculates and returns the next backoff delay.
   * @details Computes the appropriate delay for the current backoff generation and increments the
   * internal generation counter for the next call.
   * @return The calculated delay duration.
   */
  auto getNextDelay() -> std::chrono::milliseconds {
    constexpr double BACKOFF_FACTOR = 2.0;

    const auto initial_delay_ms = static_cast<double>(initial_delay_.count());

    const auto delay_ms = initial_delay_ms * std::pow(BACKOFF_FACTOR, generated_delays_);
    // Apply a positive jitter (a random value between 0 and initial_delay_)
    const auto jitter = dist_(gen_) * initial_delay_ms;

    const auto total_delay_ms = static_cast<int64_t>(delay_ms + jitter);
    const auto jittery_delay = std::chrono::milliseconds(total_delay_ms);

    generated_delays_++;

    return std::min(jittery_delay, max_delay_);
  }

  /** @brief Resets the backoff generator. */
  void reset() { generated_delays_ = 0; }

 private:
  std::chrono::milliseconds initial_delay_;
  std::chrono::milliseconds max_delay_;
  int generated_delays_{0};
  std::random_device rd_;
  std::mt19937 gen_{rd_()};
  std::uniform_real_distribution<> dist_{0.0, 1.0};
};

}  // namespace AstarteDeviceSdk

#endif  // EXPONENTIAL_BACKOFF_H
