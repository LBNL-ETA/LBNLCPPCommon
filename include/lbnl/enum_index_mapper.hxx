// enum_index_mapper.hxx
#pragma once

#include <array>
#include <optional>
#include <utility>
#include <type_traits>

namespace lbnl
{

    // Restrict to enum types (C++20 concept)
    template<class E>
    concept Enum = std::is_enum_v<E>;

    // Bidirectional mapping between DB index (int) and Enum.
    // Storage is a constexpr std::array of (index, enum) pairs.
    // Lookups are O(N) linear scans — fine for small enums and constexpr-friendly.
    template<Enum E, std::size_t N>
    class EnumIndexMapper
    {
    public:
        using pair_type = std::pair<int, E>;
        using storage_t = std::array<pair_type, N>;

        // The mapping is provided as a constexpr array of unique pairs.
        constexpr explicit EnumIndexMapper(storage_t mapping) : mapping_{mapping}
        {}

        [[nodiscard]] constexpr std::optional<E> to_enum(int index) const
        {
            for(auto const & p : mapping_)
                if(p.first == index)
                    return p.second;
            return std::nullopt;
        }

        [[nodiscard]] constexpr std::optional<int> to_index(E e) const
        {
            for(auto const & p : mapping_)
                if(p.second == e)
                    return p.first;
            return std::nullopt;
        }

        // Helpers that return fallbacks instead of optionals
        [[nodiscard]] constexpr E to_enum_or(int index, E fallback) const
        {
            if(auto v = to_enum(index))
                return *v;
            return fallback;
        }

        [[nodiscard]] constexpr int to_index_or(E e, int fallback) const
        {
            if(auto v = to_index(e))
                return *v;
            return fallback;
        }

        // Expose mapping for iteration or tests
        [[nodiscard]] constexpr storage_t const & data() const
        {
            return mapping_;
        }

    private:
        storage_t mapping_;
    };

    // Existing explicit version (keep it)
    template<Enum E, std::size_t N>
    [[nodiscard]] constexpr auto make_enum_index_mapper(const std::array<std::pair<int, E>, N> & a)
    {
        return EnumIndexMapper<E, N>{a};
    }

    // NEW overload — allows simple brace initialization
    template<Enum E, std::size_t N>
    [[nodiscard]] constexpr auto make_enum_index_mapper(const std::pair<int, E> (&a)[N])
    {
        return EnumIndexMapper<E, N>(std::to_array(a));
    }

}   // namespace lbnl
