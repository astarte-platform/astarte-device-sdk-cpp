// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_OBJECT_H
#define ASTARTE_DEVICE_SDK_OBJECT_H

/**
 * @file astarte_device_sdk/object.hpp
 * @brief Astarte object class and its related methods.
 *
 * @details This file defines the container for aggregated object datastreams,
 * allowing multiple Astarte data fields to be grouped and sent as a single coherent unit.
 */

#include <initializer_list>
#include <ostream>
#include <string>
#include <unordered_map>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

/**
 * @brief Astarte object class, representing the Astarte object datastream data.
 *
 * @details This class acts as a specialized container for mapping relative paths (keys)
 * to Astarte Data values. It is primarily used when transmitting data via interfaces
 * configured with object aggregation.
 */
class DatastreamObject {
 public:
  /// @brief Helper type for the map of paths and Astarte datas.
  using MapType = std::unordered_map<std::string, Data>;
  /// @brief Helper type for the iterator over the map of paths and Astarte datas.
  using iterator = MapType::iterator;
  /// @brief Helper type for the const iterator over the map of paths and Astarte datas.
  using const_iterator = MapType::const_iterator;
  /// @brief Helper type for size type of the map of paths and Astarte datas.
  using size_type = MapType::size_type;
  /// @brief Helper type for value type of the map of paths and Astarte datas.
  using value_type = MapType::value_type;

  /// @brief Constructs an empty DatastreamObject.
  DatastreamObject();

  /**
   * @brief Constructs a DatastreamObject with initial content.
   *
   * @param[in] init The initialize list to use as initial content.
   */
  DatastreamObject(std::initializer_list<value_type> init);

  /**
   * @brief Accesses the specified element with bounds checking.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @param[in] key The key to search for.
   * @return A reference to the value corresponding to the key.
   */
  auto at(const std::string& key) -> Data&;

  /**
   * @brief Accesses the specified element with bounds checking.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @param[in] key The key to search for.
   * @return A constant reference to the value corresponding to the key.
   */
  auto at(const std::string& key) const -> const Data&;

  /**
   * @brief Returns an iterator to the beginning of the map.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @return An iterator pointing at the beginning of the map.
   */
  auto begin() -> iterator;

  /**
   * @brief Returns an iterator to the beginning of the map.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @return A constant iterator pointing at the beginning of the map.
   */
  auto begin() const -> const_iterator;

  /**
   * @brief Returns an iterator to the end of the map.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @return An iterator pointing at the end of the map.
   */
  auto end() -> iterator;

  /**
   * @brief Returns an iterator to the end of the map.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @return A constant iterator pointing at the end of the map.
   */
  auto end() const -> const_iterator;

  /**
   * @brief Returns the number of elements in the map.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @return The number of elements in the map.
   */
  auto size() const -> size_type;

  /**
   * @brief Checks if the map is empty.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @return True if the map is empty, false otherwise.
   */
  auto empty() const -> bool;

  /**
   * @brief Inserts elements into the map.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @param[in] key The key to insert.
   * @param[in] data The value to insert.
   */
  void insert(const std::string& key, const Data& data);

  /**
   * @brief Erases elements from the map.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @param[in] key The key of the element to erase.
   * @return The number of elements removed (0 or 1).
   */
  auto erase(const std::string& key) -> size_type;

  /**
   * @brief Clears the contents of the map.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   */
  void clear();

  /**
   * @brief Finds an element with a specific key.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @param[in] key The key to find.
   * @return An iterator to the requested element.
   */
  auto find(const std::string& key) -> iterator;

  /**
   * @brief Finds an element with a specific key.
   *
   * @details Soft wrapper for the equivalent method in the std::unordered_map.
   *
   * @param[in] key The key to find.
   * @return A constant iterator to the requested element.
   */
  auto find(const std::string& key) const -> const_iterator;

  /**
   * @brief Returns the raw data contained in this class instance.
   *
   * @return A constant reference to the internal map containing the object data.
   */
  auto get_raw_data() const -> const MapType&;

  /**
   * @brief Overloader for the comparison operator ==.
   *
   * @param[in] other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const DatastreamObject& other) const -> bool;

  /**
   * @brief Overloader for the comparison operator !=.
   *
   * @param[in] other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const DatastreamObject& other) const -> bool;

 private:
  MapType data_;
};

}  // namespace astarte::device

/// @brief astarte_fmt::formatter specialization for astarte::device::DatastreamObject.
template <>
struct astarte_fmt::formatter<astarte::device::DatastreamObject> {
  /**
   * @brief Parses the format string.
   *
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the DatastreamObject object as a key-value map.
   *
   * @param[in] data The DatastreamObject instance to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output context.
   */
  template <typename FormatContext>
  auto format(const astarte::device::DatastreamObject& data, FormatContext& ctx) const {
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
 * @brief Stream insertion operator for DatastreamObject.
 *
 * @param[in,out] out The output stream.
 * @param[in] data The DatastreamObject to output.
 * @return A reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::DatastreamObject& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_OBJECT_H
