#pragma once

#include <optional>
#include <ranges>
#include <string>
#include <vector>
#include <algorithm>

// Do not create the implementation file. This is the header only library

namespace lbnl
{
    //! Finds the first element in the container that satisfies the given predicate.
    //! \tparam Container The type of the container.
    //! \tparam Predicate A callable that takes a const reference to a container element and returns
    //! a boolean.
    //! \param elements The container to search through.
    //! \param predicate The condition to be satisfied by the element.
    //! \return An optional containing the first element that satisfies the predicate, or
    //! std::nullopt if no such element exists.
    template<typename Container, std::predicate<const typename Container::value_type &> Predicate>
    [[nodiscard]] constexpr std::optional<std::decay_t<typename Container::value_type>>
      find_element(const Container & elements, Predicate predicate)
    {
        auto it = std::ranges::find_if(elements, predicate);
        if(it != std::ranges::cend(elements))
        {
            return *it;
        }
        return std::nullopt;
    }

    //! Checks if the container contains a specific value.
    //! \tparam Container The type of the container.
    //! \tparam T The type of the value to search for.
    //! \param elements The container to search through.
    //! \param value The value to search for.
    //! \return True if the value is found, false otherwise.
    template<typename Container, typename T>
    [[nodiscard]] constexpr bool contains(const Container & elements, const T & value)
    {
        return std::ranges::find(elements, value) != std::ranges::cend(elements);
    }

    //! Removes duplicate elements from a container.
    //! \tparam R The type of the range (must satisfy std::ranges::range).
    //! \param range The range of elements to process.
    //! \return A new container with duplicates removed.
    template<std::ranges::range R>
    [[nodiscard]] constexpr auto unique(const R & range)
    {
        using ValueType = std::ranges::range_value_t<R>;

        std::vector<ValueType> result(range.begin(), range.end());
        std::ranges::sort(result);
        result.erase(std::unique(result.begin(), result.end()), result.end());

        return result;
    }

    //! Combines two containers into a single container of pairs.
    //! \tparam Container1 The type of the first container.
    //! \tparam Container2 The type of the second container.
    //! \param c1 The first container.
    //! \param c2 The second container.
    //! \return A vector of pairs, where each pair contains one element from each container.
    template<std::ranges::input_range R1, std::ranges::input_range R2>
    [[nodiscard]] auto zip(R1 && r1, R2 && r2)
    {
        using T1 = std::ranges::range_value_t<R1>;
        using T2 = std::ranges::range_value_t<R2>;
        std::vector<std::pair<T1, T2>> result;

        auto it1 = std::ranges::begin(r1);
        auto it2 = std::ranges::begin(r2);
        auto end1 = std::ranges::end(r1);
        auto end2 = std::ranges::end(r2);

        for(; it1 != end1 && it2 != end2; ++it1, ++it2)
        {
            result.emplace_back(*it1, *it2);
        }
        return result;
    }


    //! Applies a transformation to elements in a range that satisfy a predicate, returning a new
    //! container.
    //!
    //! This function processes each element in the input range. If an element satisfies the given
    //! predicate, the transformation function is applied to it; otherwise, the element is copied
    //! unchanged.
    //!
    //! \tparam R The type of the range (must satisfy std::ranges::range).
    //! \tparam Predicate A callable that takes a const reference to a range element and returns a
    //! boolean.
    //! \tparam Func A callable that takes a const reference to a range element and returns a
    //! transformed value.
    //! \param range The input range to process.
    //! \param pred The predicate to determine which elements to transform.
    //! \param func The transformation function applied to elements that satisfy the predicate.
    //! \return A new vector where matching elements are transformed and others are copied as-is.
    template<std::ranges::range R, typename Predicate, typename Func>
    [[nodiscard]] constexpr auto transform_if(const R & range, Predicate pred, Func func)
    {
        using Value = std::ranges::range_value_t<R>;
        std::vector<Value> result;
        result.reserve(std::ranges::distance(range));

        for(const auto & element : range)
        {
            result.push_back(pred(element) ? func(element) : element);
        }

        return result;
    }

    //! Applies a transformation to elements in a range that satisfy a predicate.
    //!
    //! \tparam R The type of the input range (must satisfy std::ranges::range).
    //! \tparam Predicate A callable that returns true for elements to be transformed.
    //! \tparam Func A callable that transforms a matching element.
    //! \param range The input range to process.
    //! \param pred The predicate used to select elements.
    //! \param func The function used to transform selected elements.
    //! \return A vector of transformed elements that passed the predicate.
    template<std::ranges::range R, typename Predicate, typename Func>
    [[nodiscard]] auto transform_filter(const R & range, Predicate pred, Func func)
    {
        using ResultType = std::invoke_result_t<Func, std::ranges::range_value_t<R>>;
        std::vector<ResultType> result;

        for(const auto & element : range)
        {
            if(pred(element))
            {
                result.push_back(func(element));
            }
        }

        return result;
    }

    //! Filters elements in a range based on a predicate.
    //!
    //! This function iterates through the input range and includes only those elements
    //! that satisfy the given predicate.
    //!
    //! \tparam R The type of the input range (must satisfy std::ranges::range).
    //! \tparam Predicate A callable that takes a const reference to an element and returns a
    //! boolean.
    //! \param range The range of elements to filter.
    //! \param predicate The condition used to determine which elements to include.
    //! \return A vector containing the elements from the input range that satisfy the predicate.
    template<std::ranges::range R, typename Predicate>
    [[nodiscard]] constexpr auto filter(const R & range, Predicate predicate)
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

    //! Merges two sorted ranges into a single sorted range.
    //! \tparam R1 The type of the first range.
    //! \tparam R2 The type of the second range.
    //! \param range1 The first sorted range.
    //! \param range2 The second sorted range.
    //! \return A vector containing the merged sorted elements.
    template<std::ranges::range R1, std::ranges::range R2>
    [[nodiscard]] constexpr auto merge(const R1 & range1, const R2 & range2)
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

    //! Splits a string into a vector of substrings based on a delimiter.
    //! \param str The string to split.
    //! \param delimiter The character used to split the string.
    //! \return A vector of substrings.
    template<typename Str, typename CharT = typename std::decay_t<Str>::value_type>
    [[nodiscard]] constexpr std::vector<std::basic_string<CharT>> split(Str && str, CharT delimiter)
    {
        std::basic_string_view<CharT> view{std::forward<Str>(str)};
        std::vector<std::basic_string<CharT>> result;

        if(view.empty())
        {
            return result;
        }

        size_t start = 0;
        while(start <= view.size())
        {
            size_t end = view.find(delimiter, start);
            if(end == std::basic_string_view<CharT>::npos)
                end = view.size();
            result.emplace_back(view.substr(start, end - start));
            start = end + 1;
        }

        return result;
    }

    //! Partitions a range into two groups based on a predicate.
    //! \tparam R The type of the range (must satisfy std::ranges::range).
    //! \tparam Predicate A callable that takes a const reference to a range element and returns a
    //! boolean.
    //! \param range The range of elements to partition.
    //! \param predicate The condition to partition elements.
    //! \return A pair of vectors, where the first contains elements that satisfy the predicate, and
    //! the second contains the rest.
    template<std::ranges::range R, typename Predicate>
    [[nodiscard]] constexpr auto partition(const R & range, Predicate predicate)
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

    //! Flattens a nested vector into a single vector.
    //! \tparam T The type of the elements in the inner vectors.
    //! \param nested The nested vector to flatten.
    //! \return A single vector containing all elements from the nested vector.
    template<typename T>
    [[nodiscard]] constexpr std::vector<T> flatten(const std::vector<std::vector<T>> & nested)
    {
        std::vector<T> result;
        for(const auto & inner : nested)
        {
            result.insert(result.end(), inner.begin(), inner.end());
        }
        return result;
    }

    //! Converts a range into a vector.
    //! \tparam R The type of the range (must satisfy std::ranges::input_range).
    //! \param r The range to convert.
    //! \return A vector containing all elements from the range.
    template<std::ranges::input_range R>
    [[nodiscard]] constexpr auto to_vector(R && r)
    {
        using T = std::ranges::range_value_t<R>;
        return std::vector<T>(std::ranges::begin(r), std::ranges::end(r));
    }

    //! Transforms a range into a vector by applying a function to each element.
    //! \tparam R The type of the range (must satisfy std::ranges::input_range).
    //! \tparam Func A callable that takes a range element and returns a transformed value.
    //! \param range The range of elements to transform.
    //! \param func The transformation function to apply.
    //! \return A vector containing the transformed elements.
    template<std::ranges::input_range R, typename Func>
    [[nodiscard]] constexpr auto transform_to_vector(R && range, Func && func)
    {
        return to_vector(range | std::views::transform(std::forward<Func>(func)));
    }

}   // namespace lbnl