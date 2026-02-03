// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "exponential_backoff.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::AllOf;
using ::testing::Ge;
using ::testing::Le;

using astarte::device::ExponentialBackoff;

TEST(AstarteTestExponentialBackoff, IncorrectInputs) {
  ASSERT_FALSE(ExponentialBackoff::create(std::chrono::minutes(-1), std::chrono::minutes(1)));
  ASSERT_FALSE(ExponentialBackoff::create(std::chrono::minutes(1), std::chrono::minutes(-1)));
  ASSERT_FALSE(
      ExponentialBackoff::create(std::chrono::minutes(1), std::chrono::milliseconds::zero()));
  ASSERT_FALSE(
      ExponentialBackoff::create(std::chrono::milliseconds::zero(), std::chrono::minutes(1)));
}

TEST(AstarteTestExponentialBackoff, OrdinaryBackoff) {
  ExponentialBackoff backoff =
      ExponentialBackoff::create(std::chrono::minutes(1), std::chrono::minutes(18)).value();
  ASSERT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::milliseconds::zero()), Le(std::chrono::minutes(2))));
  ASSERT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::minutes(1)), Le(std::chrono::minutes(3))));
  ASSERT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::minutes(3)), Le(std::chrono::minutes(5))));
  ASSERT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::minutes(7)), Le(std::chrono::minutes(9))));
  ASSERT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::minutes(15)), Le(std::chrono::minutes(17))));
  for (size_t i = 0; i < 1048576u; i++) {
    ASSERT_THAT(backoff.getNextDelay(),
                AllOf(Ge(std::chrono::minutes(17)), Le(std::chrono::minutes(19))));
  }
}

TEST(AstarteTestExponentialBackoff, VeryLargeBackoff) {
  ExponentialBackoff backoff =
      ExponentialBackoff::create(
          std::chrono::hours(1),
          std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::years(100)))
          .value();
  ASSERT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::milliseconds::zero()), Le(std::chrono::hours(2))));
  ASSERT_THAT(backoff.getNextDelay(), AllOf(Ge(std::chrono::hours(1)), Le(std::chrono::hours(3))));
  ASSERT_THAT(backoff.getNextDelay(), AllOf(Ge(std::chrono::hours(3)), Le(std::chrono::hours(5))));
  ASSERT_THAT(backoff.getNextDelay(), AllOf(Ge(std::chrono::hours(7)), Le(std::chrono::hours(9))));
  // A lot of calls in between
  for (size_t i = 0; i < 1000000u; i++) {
    backoff.getNextDelay();
  }
  // Check it settled around the proper value
  for (size_t i = 0; i < 100u; i++) {
    ASSERT_THAT(
        backoff.getNextDelay(),
        AllOf(Ge(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::years(99))),
              Le(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::years(101)))));
  }
}
