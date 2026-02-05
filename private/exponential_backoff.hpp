// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef EXPONENTIAL_BACKOFF_H
#define EXPONENTIAL_BACKOFF_H

/**
 * @file private/exponential_backoff.hpp
 * @brief Exponential backoff generator with jitter.
 *
 * @details This file defines a utility class for calculating wait times between
 * retry attempts, using an exponential backoff strategy with randomized jitter
 * to prevent thundering herd problems.
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <random>

#include "astarte_device_sdk/errors.hpp"

namespace astarte::device {

/// @brief Class for generating exponential backoff delays with jitter.
class ExponentialBackoff {
 public:
  /**
   * @brief Creates an ExponentialBackoff instance.
   *
   * @details The exponential backoff will compute an exponential delay using
   * 2 as the base for the power operation and @p mul_coeff as the multiplier coefficient.
   * The values returned by calls to getNextDelay will follow the formula:
   * min( @p mul_coeff * 2 ^ ( number of calls ) , @p cutoff_coeff ) + random jitter.
   * The random jitter will be in the range [ - @p mul_coeff , + @p mul_coeff ].
   *
   * @note The jitter will be applied also once the @p cutoff_coeff has been reached. Effectively
   * the maximum delay produced will be @p cutoff_coeff + @p mul_coeff.
   *
   * @param[in] mul_coeff Multiplier coefficient used in the exponential delay calculation.
   * @param[in] cutoff_coeff The cut-off coefficient, an upper bound for the exponential curve.
   * @return An expected containing the ExponentialBackoff instance on success or Error on failure.
   */
  static auto create(std::chrono::milliseconds mul_coeff, std::chrono::milliseconds cutoff_coeff)
      -> astarte_tl::expected<ExponentialBackoff, Error> {
    if ((mul_coeff <= std::chrono::milliseconds::zero()) ||
        (cutoff_coeff <= std::chrono::milliseconds::zero())) {
      return astarte_tl::unexpected(
          InvalidInputError{"ExponentialBackoff create() received zero or negative coefficients"});
    }
    if (cutoff_coeff < mul_coeff) {
      return astarte_tl::unexpected(
          InvalidInputError{"ExponentialBackoff create() received a multiplier coefficient "
                            "larger than the cutoff coefficient"});
    }
    return ExponentialBackoff(mul_coeff, cutoff_coeff);
  }

  /**
   * @brief Calculates and returns the next backoff delay.
   *
   * @details Computes the next delay duration based on the number of previous calls
   * since the last reset, clamped by the cutoff coefficient, and modifies it with
   * random jitter.
   *
   * @return The calculated delay duration in milliseconds.
   */
  auto getNextDelay() -> std::chrono::milliseconds {
    const ChronoMillisRep mul_coeff = mul_coeff_.count();
    const ChronoMillisRep max_milliseconds = std::chrono::milliseconds::max().count();
    const ChronoMillisRep max_allowed_final_delay = max_milliseconds - mul_coeff;

    // Update last delay value with the new value
    ChronoMillisRep delay = 0;
    if (prev_delay_ == 0) {
      delay = mul_coeff;
    } else if (prev_delay_ <= (max_allowed_final_delay / 2)) {
      delay = 2 * prev_delay_;
    } else {
      delay = max_allowed_final_delay;
    }

    // Bound the delay to the maximum
    const ChronoMillisRep bounded_delay = std::min(delay, cutoff_coeff_.count());

    // Store the new delay before jitter application
    prev_delay_ = bounded_delay;

    // Insert some jitter
    ChronoMillisRep jitter_minimum = -mul_coeff;
    if (bounded_delay - mul_coeff < 0) {
      jitter_minimum = 0;
    }
    ChronoMillisRep jitter_maximum = mul_coeff;
    if (bounded_delay > max_milliseconds - mul_coeff) {
      jitter_maximum = max_milliseconds - bounded_delay;
    }
    std::uniform_int_distribution<ChronoMillisRep> dist(jitter_minimum, jitter_maximum);
    const ChronoMillisRep jittered_delay = bounded_delay + dist(gen_);

    // Convert to a chrono object
    return std::chrono::milliseconds(jittered_delay);
  }

  /**
   * @brief Resets the backoff generator.
   *
   * @details Resets the internal state so that the next call to getNextDelay()
   * will start from the initial multiplier coefficient.
   */
  void reset() { prev_delay_ = 0; }

 private:
  using ChronoMillisRep = std::chrono::milliseconds::rep;

  /**
   * @brief Constructs an ExponentialBackoff instance.
   *
   * @param[in] mul_coeff Multiplier coefficient used in the exponential delay calculation.
   * @param[in] cutoff_coeff The cut-off coefficient, an upper bound for the exponential curve.
   */
  ExponentialBackoff(std::chrono::milliseconds mul_coeff, std::chrono::milliseconds cutoff_coeff)
      : mul_coeff_(mul_coeff), cutoff_coeff_(cutoff_coeff) {}

  std::chrono::milliseconds mul_coeff_;
  std::chrono::milliseconds cutoff_coeff_;
  std::mt19937 gen_{std::random_device{}()};
  ChronoMillisRep prev_delay_{0};
};

}  // namespace astarte::device

#endif  // EXPONENTIAL_BACKOFF_H
