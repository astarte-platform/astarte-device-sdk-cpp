// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_OBJECT_H
#define ASTARTE_DEVICE_SDK_OBJECT_H

/**
 * @file astarte_device_sdk/object.hpp
 * @brief Astarte object class and its related methods.
 */

#include <initializer_list>
#include <string>
#include <unordered_map>

#include "astarte_device_sdk/individual.hpp"

namespace AstarteDeviceSdk {

/** @brief Astarte object class, representing the Astarte object datastream data. */
class AstarteObject {
 public:
  /** @brief Helper type for the map of paths and Astarte individuals. */
  using MapType = std::unordered_map<std::string, AstarteIndividual>;
  /** @brief Helper type for the iterator over the map of paths and Astarte individuals. */
  using iterator = MapType::iterator;
  /** @brief Helper type for the const iterator over the map of paths and Astarte individuals. */
  using const_iterator = MapType::const_iterator;
  /** @brief Helper type for size type of the map of paths and Astarte individuals. */
  using size_type = MapType::size_type;
  /** @brief Helper type for value type of the map of paths and Astarte individuals. */
  using value_type = MapType::value_type;

  /** @brief Constructor for the class. To instantiate an empty object. */
  AstarteObject();
  /**
   * @brief Constructor for the class. To instantiate a non-empty object.
   * @param init The initialize list to use as intial content.
   */
  AstarteObject(std::initializer_list<value_type> init);
  /**
   * @brief Access specified element with bounds checking.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @param key The key to search for.
   * @return Reference to the value corresponding to the key.
   */
  auto at(const std::string& key) -> AstarteIndividual&;
  /**
   * @brief Access specified element with bounds checking.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @param key The key to search for.
   * @return Reference to the value corresponding to the key.
   */
  auto at(const std::string& key) const -> const AstarteIndividual&;
  /**
   * @brief Returns an iterator to the beginning of the specified bucket.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @return Iterator pointing at the beginning of the map.
   */
  auto begin() -> iterator;
  /**
   * @brief Returns an iterator to the beginning of the specified bucket.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @return Iterator pointing at the beginning of the map.
   */
  auto begin() const -> const_iterator;
  /**
   * @brief Returns an iterator to the end of the specified bucket.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @return Iterator pointing at the end of the map.
   */
  auto end() -> iterator;
  /**
   * @brief Returns an iterator to the end of the specified bucket.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @return Iterator pointing at the end of the map.
   */
  auto end() const -> const_iterator;
  /**
   * @brief Returns the number of elements.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @return Number of elements in the map.
   */
  auto size() const -> size_type;
  /**
   * @brief Checks whether the container is empty.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @return True if the map is empty, false otherwise.
   */
  auto empty() const -> bool;
  /**
   * @brief Insert elements.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @param key Key to insert.
   * @param individual Value to insert.
   */
  void insert(const std::string& key, const AstarteIndividual& individual);
  /**
   * @brief Erases elements.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @param key Key to erase.
   * @return Number of elements removed (0 or 1).
   */
  auto erase(const std::string& key) -> size_type;
  /**
   * @brief Clears the contents .
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   */
  void clear();
  /**
   * @brief Finds element with specific key.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @param key Key to find.
   * @return An iterator to the requested element.
   */
  auto find(const std::string& key) -> iterator;
  /**
   * @brief Finds element with specific key.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @param key Key to find.
   * @return An iterator to the requested element.
   */
  auto find(const std::string& key) const -> const_iterator;
  /**
   * @brief Pretty format the Astarte object.
   * @return A string representing in human readable format the content of the class instance.
   */
  auto format() const -> std::string;

 private:
  MapType data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_OBJECT_H
