#pragma once

#include <optional>
#include <type_traits>
#include <utility>

namespace lbnl
{
    // `and_then`: Calls function if optional contains a value
    template<typename T, typename Func>
    auto and_then(const std::optional<T>& opt,
                  Func&& func) -> std::optional<std::invoke_result_t<Func, const T&>>
    {
        if (opt)
        {
            return std::invoke(std::forward<Func>(func), *opt);
        }
        return std::nullopt;
    }

    // `or_else`: Calls function if optional is empty
    template<typename T, typename Func>
    auto or_else(const std::optional<T>& opt, Func&& func) -> T
    {
        return opt ? *opt : std::invoke(std::forward<Func>(func));
    }

    // Define `operator|` for `std::optional<T>`
    template<typename T, typename Func>
    auto operator|(const std::optional<T>& opt,
                   Func&& func) -> std::optional<std::invoke_result_t<Func, const T&>>
    {
        return and_then(opt, std::forward<Func>(func));
    }

    // Define `operator||` for `std::optional<T>`
    template<typename T, typename Func>
    auto operator||(const std::optional<T>& opt, Func&& func) -> T
    {
        return or_else(opt, std::forward<Func>(func));
    }

    template<typename T>
    struct is_std_optional : std::false_type {};

    template<typename T>
    struct is_std_optional<std::optional<T>> : std::true_type {};


    template<typename T>
    struct is_optional_ext;  // forward declare, full definition later

    // OptionalExt for monadic-like chaining
    template<typename T>
    class OptionalExt
    {
    public:
        explicit OptionalExt(std::optional<T> opt) : m_opt(std::move(opt)) {}

        template<typename Func>
        auto and_then(Func&& f) const
        {
            using RawResult = std::invoke_result_t<Func, const T&>;

            if (!m_opt)
            {
                if constexpr (std::is_void_v<RawResult>)
                {
                    return OptionalExt<std::monostate>(std::nullopt);
                }
                else if constexpr (is_std_optional<RawResult>::value)
                {
                    using Inner = typename RawResult::value_type;
                    return OptionalExt<Inner>(std::nullopt);
                }
                else if constexpr (lbnl::is_optional_ext<RawResult>::value)
                {
                    using Inner = typename RawResult::value_type;
                    return OptionalExt<Inner>(std::nullopt);
                }
                else
                {
                    return OptionalExt<RawResult>(std::nullopt);
                }
            }

            if constexpr (std::is_void_v<RawResult>)
            {
                std::invoke(std::forward<Func>(f), *m_opt);
                return OptionalExt<std::monostate>(std::monostate{});
            }
            else
            {
                auto result = std::invoke(std::forward<Func>(f), *m_opt);

                if constexpr (is_std_optional<RawResult>::value)
                {
                    using Inner = typename RawResult::value_type;
                    return OptionalExt<Inner>(result);
                }
                else if constexpr (lbnl::is_optional_ext<RawResult>::value)
                {
                    return result;
                }
                else
                {
                    return OptionalExt<RawResult>(result);
                }
            }
        }

        template<typename Func>
        auto or_else(Func&& f) const
        {
            using Result = std::invoke_result_t<Func>;

            if constexpr (std::is_void_v<Result>)
            {
                if (!m_opt)
                {
                    std::invoke(std::forward<Func>(f));
                    return OptionalExt<std::monostate>(std::monostate{});
                }
                return OptionalExt<std::monostate>(std::monostate{});
            }
            else
            {
                using ReturnType = std::decay_t<Result>;
                if (m_opt)
                {
                    return OptionalExt<ReturnType>(*m_opt);
                }
                else
                {
                    return OptionalExt<ReturnType>(std::invoke(std::forward<Func>(f)));
                }
            }
        }

        const std::optional<T>& raw() const
        {
            return m_opt;
        }

    private:
        std::optional<T> m_opt;
    };

    // Trait must be declared *after* OptionalExt is defined
    template<typename T>
    struct is_optional_ext : std::false_type {};

    template<typename T>
    struct is_optional_ext<lbnl::OptionalExt<T>> : std::true_type {};

    template<typename T>
    auto extend(const std::optional<T>& opt)
    {
        return OptionalExt<T>(opt);
    }

} // namespace lbnl
