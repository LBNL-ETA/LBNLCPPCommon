#pragma once

#include <optional>
#include <type_traits>
#include <utility>

namespace lbnl
{
    // and_then: Calls function if value is present, returns new optional
    template<typename T, typename Func>
    auto and_then(const std::optional<T> & opt, Func && func)
      -> std::optional<std::invoke_result_t<Func, const T &>>   // Fix deduction
    {
        if(opt)
        {
            return std::invoke(std::forward<Func>(func), *opt);
        }
        return std::nullopt;
    }

    // or_else: Calls function if optional is empty, returns default optional
    template<typename T, typename Func>
    auto or_else(const std::optional<T> & opt, Func && func) -> std::optional<T>
    {
        if(!opt)
        {
            return std::invoke(std::forward<Func>(func));
        }
        return opt;
    }

}   // namespace lbnl