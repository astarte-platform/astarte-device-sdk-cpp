// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_INTROSPECTION_H
#define ASTARTE_INTROSPECTION_H

/**
 * @file private/mqtt/introspection.hpp
 * @brief Thread-safe container for device introspection.
 *
 * @details This file defines the `Introspection` class, which holds the set of interfaces
 * supported by the device, managing concurrent access and lookup.
 */

#include <map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <vector>

#include "astarte_device_sdk/errors.hpp"
#include "mqtt/interface.hpp"

namespace astarte::device::mqtt {

/**
 * @brief Thread-safe collection of Astarte interfaces.
 *
 * @details The introspection represents the complete set of interfaces the device supports.
 * It provides methods to add, retrieve, and enumerate interfaces safely in a concurrent
 * environment.
 */
class Introspection {
 public:
  /// @brief Default constructor.
  Introspection() = default;

  /// @brief Default destructor.
  ~Introspection() = default;

  /// @brief Introspection is non-copyable.
  Introspection(const Introspection&) = delete;

  /// @brief Introspection is non-copyable.
  auto operator=(const Introspection&) -> Introspection& = delete;

  /// @brief Introspection is non-moveable.
  Introspection(Introspection&&) = delete;

  /// @brief Introspection is non-moveable.
  auto operator=(Introspection&&) -> Introspection& = delete;

  /**
   * @brief Adds an Interface to the Introspection safely.
   *
   * @param[in] interface The interface object to add.
   * @return An expected containing void on success or Error on failure.
   */
  auto checked_insert(Interface interface) -> astarte_tl::expected<void, Error>;

  /**
   * @brief Returns a snapshot of the introspection values as shared pointers.
   *
   * @return A vector containing shared pointers to the currently installed interfaces.
   */
  [[nodiscard]] auto values() const -> std::vector<std::shared_ptr<const Interface>>;

  /**
   * @brief Gets a read-only, thread-safe handle to an interface.
   *
   * @param[in] interface_name The name of the interface to retrieve.
   * @return An expected containing the pointer to the Interface on success or Error on failure.
   */
  [[nodiscard]] auto get(std::string_view interface_name) const
      -> astarte_tl::expected<std::shared_ptr<const Interface>, Error>;

 private:
  mutable std::shared_mutex lock_;
  std::map<std::string, std::shared_ptr<const Interface>, std::less<>> interfaces_;
};

}  // namespace astarte::device::mqtt

#endif  // ASTARTE_INTROSPECTION_H
