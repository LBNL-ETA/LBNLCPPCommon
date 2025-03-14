#pragma once

#include <optional>
#include <ranges>

namespace lbnl 
{
    
    template<typename Container, std::predicate<const typename Container::value_type&> Predicate>
    std::optional<typename Container::value_type> findElement(const Container& elements, Predicate predicate) {
        const auto it = std::ranges::find_if(elements, predicate);
        return (it != std::ranges::cend(elements)) ? std::optional<typename Container::value_type>(*it) : std::nullopt;
    }

}  // namespace lbnl