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

    // Helper class to extend `std::optional` with `and_then` and `or_else` methods
    // This can be removed once C++23 is available
    template<typename T>
    class OptionalExt
    {
    public:
        explicit OptionalExt(std::optional<T> opt) : m_opt(std::move(opt))
        {}

        template<typename Func>
        auto and_then(Func && f) const
        {
            using Result = std::invoke_result_t<Func, const T &>;
            if(m_opt)
            {
                return OptionalExt<Result>(std::invoke(std::forward<Func>(f), *m_opt));
            }
            return OptionalExt<Result>(std::nullopt);
        }

        template<typename Func>
        auto or_else(Func && f) const -> T
        {
            return m_opt ? *m_opt : std::invoke(std::forward<Func>(f));
        }

        const std::optional<T> & raw() const
        {
            return m_opt;
        }

    private:
        std::optional<T> m_opt;
    };

    template<typename T>
    auto extend(const std::optional<T> & opt)
    {
        return OptionalExt<T>(opt);
    }

}   // namespace lbnl
