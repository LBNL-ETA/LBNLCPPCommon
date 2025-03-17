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
    auto unique(const R & range)
    {
        using ValueType = std::ranges::range_value_t<R>;

        std::vector<ValueType> result(range.begin(), range.end());
        std::sort(result.begin(), result.end());
        result.erase(std::unique(result.begin(), result.end()), result.end());

        return result;
    }

    template<typename Container1, typename Container2>
    auto zip(const Container1 & c1, const Container2 & c2)
    {
        std::vector<std::pair<typename Container1::value_type, typename Container2::value_type>>
          result;
        auto size = std::min(c1.size(), c2.size());

        for(size_t i = 0; i < size; ++i)
        {
            result.emplace_back(c1[i], c2[i]);
        }
        return result;
    }

    template<std::ranges::range R, typename Predicate>
    auto filter(const R & range, Predicate predicate)
    {
        std::vector<std::ranges::range_value_t<R>> result;
        std::ranges::for_each(range, [&](const auto & element) {
            if(predicate(element))
            {
                result.push_back(element);
            }
        });
        return result;
    }

    template<std::ranges::range R1, std::ranges::range R2>
    auto merge(const R1 & range1, const R2 & range2)
    {
        std::vector<std::ranges::range_value_t<R1>> result;
        auto it1 = range1.begin();
        auto it2 = range2.begin();
        while(it1 != range1.end() && it2 != range2.end())
        {
            if(*it1 <= *it2)
            {
                result.push_back(*it1);
                ++it1;
            }
            else
            {
                result.push_back(*it2);
                ++it2;
            }
        }
        while(it1 != range1.end())
        {
            result.push_back(*it1);
            ++it1;
        }
        while(it2 != range2.end())
        {
            result.push_back(*it2);
            ++it2;
        }
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

    template<std::ranges::range R, typename Predicate>
    auto partition(const R & range, Predicate predicate)
    {
        std::pair<std::vector<std::ranges::range_value_t<R>>,
                  std::vector<std::ranges::range_value_t<R>>>
          result;
        for(const auto & element : range)
        {
            if(predicate(element))
            {
                result.first.push_back(element);
            }
            else
            {
                result.second.push_back(element);
            }
        }
        return result;
    }

    template<typename T>
    std::vector<T> flatten(const std::vector<std::vector<T>> & nested)
    {
        std::vector<T> result;
        for(const auto & inner : nested)
        {
            result.insert(result.end(), inner.begin(), inner.end());
        }
        return result;
    }


}   // namespace lbnl