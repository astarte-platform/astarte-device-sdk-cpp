// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef SHARED_QUEUE_H
#define SHARED_QUEUE_H

/**
 * @file private/shared_queue.hpp
 * @brief Thread-safe queue implementation.
 *
 * @details This file defines a thread-safe queue wrapper using std::queue, std::mutex,
 * and std::condition_variable to allow safe concurrent access and blocking operations.
 */

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>

namespace astarte::device {

/**
 * @brief A thread-safe queue template.
 *
 * @details This class wraps a standard queue to provide thread-safe push and pop operations.
 * It uses a mutex to protect shared data and a condition variable to support blocking
 * reads with timeouts.
 *
 * @tparam T The type of elements stored in the queue.
 */
template <typename T>
class SharedQueue {
 public:
  /**
   * @brief Pops an element from the queue with a timeout.
   *
   * @details This method blocks the calling thread until an element is available or the
   * specified timeout expires.
   *
   * @param[in] timeout The maximum duration to wait for an item.
   * @return std::optional containing the item if retrieved, or std::nullopt if the
   * timeout was reached or the queue remained empty.
   */
  auto pop(const std::chrono::milliseconds& timeout) -> std::optional<T> {
    std::unique_lock<std::mutex> mlock(mutex_);
    if (condition_.wait_for(mlock, timeout, [this] { return !queue_.empty(); })) {
      T res = queue_.front();
      queue_.pop();
      return res;
    }
    return std::nullopt;
  }

  /**
   * @brief Pushes a new element into the queue.
   *
   * @details This method locks the queue, adds the item, and notifies one waiting thread.
   *
   * @param[in] item The item to be added to the queue.
   */
  void push(const T& item) {
    const std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    condition_.notify_one();
  }

  /**
   * @brief Returns the number of elements in the queue.
   *
   * @details This operation is thread-safe.
   *
   * @return The number of elements currently in the queue.
   */
  auto size() -> std::size_t {
    std::unique_lock<std::mutex> mlock(mutex_);
    return queue_.size();
  }

  /**
   * @brief Checks if the queue is empty.
   *
   * @details This operation is thread-safe.
   *
   * @return True if the queue contains no elements, false otherwise.
   */
  auto empty() -> bool {
    std::unique_lock<std::mutex> mlock(mutex_);
    return queue_.empty();
  }

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable condition_;
};

}  // namespace astarte::device

#endif  // SHARED_QUEUE_H
