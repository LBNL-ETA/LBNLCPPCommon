#pragma once

#include <optional>
#include <vector>

namespace lbnl
{
    template<typename T>
    std::optional<T> averageOptional(const std::vector<std::optional<T>> & values)
    {
        T sum{};
        size_t count = 0;

        for(const auto & val : values)
        {
            if(val)
            {
                sum += *val;
                ++count;
            }
        }

        if(count == 0)
            return std::nullopt;

        return sum / static_cast<T>(count);
    }

}   // namespace lbnl
