#pragma once

#include <optional>
#include <type_traits>
#include <utility>

namespace lbnl
{
    // `and_then`: Calls function if optional contains a value
    template<typename T, typename Func>
    auto and_then(const std::optional<T> & opt,
                  Func && func) -> std::optional<std::invoke_result_t<Func, const T &>>
    {
        if(opt)
        {
            return std::invoke(std::forward<Func>(func), *opt);
        }
        return std::nullopt;
    }

    // `or_else`: Calls function if optional is empty
    template<typename T, typename Func>
    auto or_else(const std::optional<T> & opt, Func && func) -> T
    {
        return opt ? *opt : std::invoke(std::forward<Func>(func));
    }

    // Define `operator|` for `std::optional<T>`
    template<typename T, typename Func>
    auto operator|(const std::optional<T> & opt,
                   Func && func) -> std::optional<std::invoke_result_t<Func, const T &>>
    {
        return and_then(opt, std::forward<Func>(func));
    }

    // Define `operator||` for `std::optional<T>`
    template<typename T, typename Func>
    auto operator||(const std::optional<T> & opt, Func && func) -> T
    {
        return or_else(opt, std::forward<Func>(func));
    }

}   // namespace lbnl
