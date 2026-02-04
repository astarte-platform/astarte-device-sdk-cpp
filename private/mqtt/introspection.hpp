// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_INTROSPECTION_H
#define ASTARTE_INTROSPECTION_H

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
 * @brief Thread safe collection of Astarte interface.
 *
 * The introspection represents the set of device supported interfaces.
 */
class Introspection {
 public:
  /** @brief Default constructor. */
  Introspection() = default;
  /** @brief Default destructor. */
  ~Introspection() = default;
  /** @brief Deleted copy constructor. */
  Introspection(const Introspection&) = delete;
  /** @brief Deleted copy assignment operator. */
  auto operator=(const Introspection&) -> Introspection& = delete;
  /** @brief Deleted move constructor. */
  Introspection(Introspection&&) = delete;
  /** @brief Deleted move assignment operator. */
  auto operator=(Introspection&&) -> Introspection& = delete;

  /**
   * @brief Try to insert an Interface into the Introspection.
   *
   * @param interface The interface to add.
   * @return an error if the operation fails
   */
  auto checked_insert(Interface interface) -> astarte_tl::expected<void, Error>;

  /**
   * @brief Return a snapshot of the introspection values as shared pointers.
   *
   * @return a vector containing shared pointers to the introspection interfaces.
   */
  [[nodiscard]] auto values() const -> std::vector<std::shared_ptr<const Interface>>;

  /**
   * @brief Get a read-only, thread-safe handle to an interface.
   *
   * @param interface_name the interface name.
   * @return a shared pointer to the const interface if found, an error otherwise.
   */
  [[nodiscard]] auto get(std::string_view interface_name) const
      -> astarte_tl::expected<std::shared_ptr<const Interface>, Error>;

 private:
  mutable std::shared_mutex lock_;
  std::map<std::string, std::shared_ptr<const Interface>, std::less<>> interfaces_;
};

}  // namespace astarte::device::mqtt

#endif  // ASTARTE_INTROSPECTION_H
