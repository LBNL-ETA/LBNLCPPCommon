#pragma once

#include <optional>
#include <ranges>
#include <vector>

namespace lbnl
{
    //
    // Concept for containers like std::map or std::unordered_map
    //
    template<typename Map>
    concept AssociativeContainer =
      requires(Map m, typename Map::key_type key, typename Map::mapped_type value) {
          { m.find(key) } -> std::same_as<typename Map::iterator>;
          { m.begin() } -> std::same_as<typename Map::iterator>;
          { m.end() } -> std::same_as<typename Map::iterator>;
      };

    //
    // Returns an optional value if the key exists in the map.
    //
    template<AssociativeContainer Map>
    [[nodiscard]] constexpr auto map_lookup_by_key(const Map& m, const typename Map::key_type& key)
      -> std::optional<typename Map::mapped_type>
    {
        auto it = m.find(key);
        if (it != m.end())
            return it->second;
        return std::nullopt;
    }

    //
    // Returns an optional key if a given value exists in the map.
    //
    template<AssociativeContainer Map>
    [[nodiscard]] constexpr auto map_lookup_by_value(const Map& m, const typename Map::mapped_type& value)
      -> std::optional<typename Map::key_type>
    {
        using Pair = typename Map::value_type;
        auto it = std::ranges::find(m, value, &Pair::second);
        if (it != m.end())
            return it->first;
        return std::nullopt;
    }

    //
    // Extracts all keys from the map as a vector.
    //
    template<AssociativeContainer Map>
    [[nodiscard]] constexpr auto map_keys(const Map& m) -> std::vector<typename Map::key_type>
    {
        std::vector<typename Map::key_type> keys;
        keys.reserve(m.size());
        for (const auto& [key, _] : m)
        {
            keys.push_back(key);
        }
        return keys;
    }

    //
    // Extracts all values from the map as a vector.
    //
    template<AssociativeContainer Map>
    [[nodiscard]] constexpr auto map_values(const Map& m) -> std::vector<typename Map::mapped_type>
    {
        std::vector<typename Map::mapped_type> values;
        values.reserve(m.size());
        for (const auto& [_, value] : m)
        {
            values.push_back(value);
        }
        return values;
    }
}