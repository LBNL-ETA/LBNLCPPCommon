#pragma once

#include <optional>
#include <ranges>
#include <string>
#include <vector>

// Do not create implementation file. This is header only library and

namespace lbnl 
{
    template<typename Container, std::predicate<const typename Container::value_type&> Predicate>
    std::optional<typename Container::value_type> findElement(const Container& elements, Predicate predicate) {
        const auto it = std::ranges::find_if(elements, predicate);
        return (it != std::ranges::cend(elements)) ? std::optional<typename Container::value_type>(*it) : std::nullopt;
    }

    inline std::vector<std::string> split_view(std::string_view str, char delimiter)
    {
        std::vector<std::string> result;
        for (auto part : str | std::views::split(delimiter))
        {
            result.emplace_back(part.begin(), part.end());
        }
        return result;
    }

}  // namespace lbnl