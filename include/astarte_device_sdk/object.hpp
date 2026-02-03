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
#include <ostream>
#include <string>
#include <unordered_map>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

/** @brief Astarte object class, representing the Astarte object datastream data. */
class AstarteDatastreamObject {
 public:
  /** @brief Helper type for the map of paths and Astarte datas. */
  using MapType = std::unordered_map<std::string, Data>;
  /** @brief Helper type for the iterator over the map of paths and Astarte datas. */
  using iterator = MapType::iterator;
  /** @brief Helper type for the const iterator over the map of paths and Astarte datas. */
  using const_iterator = MapType::const_iterator;
  /** @brief Helper type for size type of the map of paths and Astarte datas. */
  using size_type = MapType::size_type;
  /** @brief Helper type for value type of the map of paths and Astarte datas. */
  using value_type = MapType::value_type;

  /** @brief Constructor for the class. To instantiate an empty object. */
  AstarteDatastreamObject();
  /**
   * @brief Constructor for the class. To instantiate a non-empty object.
   * @param init The initialize list to use as intial content.
   */
  AstarteDatastreamObject(std::initializer_list<value_type> init);
  /**
   * @brief Access specified element with bounds checking.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @param key The key to search for.
   * @return Reference to the value corresponding to the key.
   */
  auto at(const std::string& key) -> Data&;
  /**
   * @brief Access specified element with bounds checking.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @param key The key to search for.
   * @return Reference to the value corresponding to the key.
   */
  auto at(const std::string& key) const -> const Data&;
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
   * @param data Value to insert.
   */
  void insert(const std::string& key, const Data& data);
  /**
   * @brief Erases elements.
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   * @param key Key to erase.
   * @return Number of elements removed (0 or 1).
   */
  auto erase(const std::string& key) -> size_type;
  /**
   * @brief Clears the contents.
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
   * @brief Return the raw data contained in this class instance.
   * @return The raw data contained in this class instance.
   */
  auto get_raw_data() const -> const MapType&;
  /**
   * @brief Overloader for the comparison operator ==.
   * @param other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const AstarteDatastreamObject& other) const -> bool;
  /**
   * @brief Overloader for the comparison operator !=.
   * @param other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const AstarteDatastreamObject& other) const -> bool;

 private:
  MapType data_;
};

}  // namespace astarte::device

/**
 * @brief astarte_fmt::formatter specialization for
 * astarte::device::AstarteDatastreamObject.
 */
template <>
struct astarte_fmt::formatter<astarte::device::AstarteDatastreamObject> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the AstarteDatastreamObject object as a key-value map.
   * @param data The AstarteDatastreamObject to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::AstarteDatastreamObject& data, FormatContext& ctx) const {
    auto out = ctx.out();
    out = astarte_fmt::format_to(out, "{{");

    bool first = true;
    for (const auto& pair : data.get_raw_data()) {
      if (!first) {
        out = astarte_fmt::format_to(out, ", ");
      }
      out = astarte_fmt::format_to(out, R"("{}": {})", pair.first, pair.second);
      first = false;
    }

    out = astarte_fmt::format_to(out, "}}");
    return out;
  }
};

/**
 * @brief Stream insertion operator for AstarteDatastreamObject.
 * @param out The output stream.
 * @param data The AstarteDatastreamObject to output.
 * @return Reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::AstarteDatastreamObject& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_OBJECT_H
