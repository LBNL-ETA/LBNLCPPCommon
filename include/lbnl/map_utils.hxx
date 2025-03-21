#pragma once

#include <optional>
#include <ranges>
#include <vector>

namespace lbnl
{
    template<typename Map>
    concept AssociativeContainer =
      requires(Map m, typename Map::key_type key, typename Map::mapped_type value) {
          { m.find(key) } -> std::same_as<typename Map::iterator>;
          { m.end() } -> std::same_as<typename Map::iterator>;
          { m.begin() } -> std::same_as<typename Map::iterator>;
      };

    template<AssociativeContainer Map>
    auto mapLookupKey(const Map & m, const typename Map::key_type & key)
      -> std::optional<typename Map::mapped_type>
    {
        auto it = m.find(key);
        if(it != m.end())
            return it->second;
        return std::nullopt;
    }

    template<AssociativeContainer Map>
    auto mapLookupValue(const Map & m, const typename Map::mapped_type & value)
      -> std::optional<typename Map::key_type>
    {
        auto it = std::ranges::find(
          m, value, &std::pair<const typename Map::key_type, typename Map::mapped_type>::second);
        if(it != m.end())
            return it->first;
        return std::nullopt;
    }

    template<AssociativeContainer Map>
    auto mapKeys(const Map & m) -> std::vector<typename Map::key_type>
    {
        std::vector<typename Map::key_type> keys;
        keys.reserve(m.size());
        for(const auto & pair : m)
        {
            keys.push_back(pair.first);
        }
        return keys;
    }

    template<AssociativeContainer Map>
    auto mapValues(const Map & m) -> std::vector<typename Map::mapped_type>
    {
        std::vector<typename Map::mapped_type> values;
        values.reserve(m.size());
        for(const auto & pair : m)
        {
            values.push_back(pair.second);
        }
        return values;
    }
}   // namespace lbnl