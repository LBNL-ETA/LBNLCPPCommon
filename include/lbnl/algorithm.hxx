#pragma once

#include <optional>
#include <ranges>
#include <string>
#include <vector>
#include <algorithm>

// Do not create implementation file. This is header only library and

namespace lbnl
{
    template<typename Container, std::predicate<const typename Container::value_type &> Predicate>
    std::optional<typename Container::value_type> findElement(const Container & elements,
                                                              Predicate predicate)
    {
        const auto it = std::ranges::find_if(elements, predicate);
        return (it != std::ranges::cend(elements))
                 ? std::optional<typename Container::value_type>(*it)
                 : std::nullopt;
    }

    //! Check if the container has a value
    template<typename Container, typename T>
    bool contains(const Container & elements, const T & value)
    {
        return std::ranges::find(elements, value) != std::ranges::cend(elements);
    }

    //! Remove duplicates from a container
    template<std::ranges::range R>
    auto unique(const R& range)
    {
        using ValueType = std::ranges::range_value_t<R>;

        std::vector<ValueType> result(range.begin(), range.end());
        std::sort(result.begin(), result.end());
        result.erase(std::unique(result.begin(), result.end()), result.end());

        return result;
    }

    inline std::vector<std::string> split(std::string_view str, char delimiter)
    {
        std::vector<std::string> result;
        for(auto part : str | std::views::split(delimiter))
        {
            result.emplace_back(part.begin(), part.end());
        }
        return result;
    }

}   // namespace lbnl