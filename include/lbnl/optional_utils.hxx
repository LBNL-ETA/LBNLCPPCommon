#pragma once

#include <optional>
#include <vector>

namespace lbnl
{
    //
    // Computes the average of a vector of std::optional<T>.
    // Ignores nullopt entries. Returns nullopt if all values are empty.
    //
    // Requires:
    // - T must support default construction, operator+=, and operator/ with scalar.
    //
    template<typename T>
    [[nodiscard]] constexpr std::optional<T> average_optional(const std::vector<std::optional<T>>& values)
    {
        T sum{};
        size_t count = 0;

        for (const auto& val : values)
        {
            if (val)
            {
                sum += *val;
                ++count;
            }
        }

        if (count == 0)
        {
            return std::nullopt;
        }

        return sum / static_cast<T>(count);
    }

} // namespace lbnl
