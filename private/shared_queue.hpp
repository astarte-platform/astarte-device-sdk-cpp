// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef SHARED_QUEUE_H
#define SHARED_QUEUE_H

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>

namespace AstarteDeviceSdk {

template <typename T>
class SharedQueue {
 public:
  auto pop(const std::chrono::milliseconds& timeout) -> std::optional<T> {
    std::unique_lock<std::mutex> mlock(mutex_);
    if (condition_.wait_for(mlock, timeout, [this] { return !queue_.empty(); })) {
      T res = queue_.front();
      queue_.pop();
      return res;
    }
    return std::nullopt;
  }
  void push(const T& item) {
    const std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    condition_.notify_one();
  }
  auto size() -> std::size_t {
    std::unique_lock<std::mutex> mlock(mutex_);
    return queue_.size();
  }
  auto empty() -> bool {
    std::unique_lock<std::mutex> mlock(mutex_);
    return queue_.empty();
  }

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable condition_;
};

}  // namespace AstarteDeviceSdk

#endif  // SHARED_QUEUE_H
