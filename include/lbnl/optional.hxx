#pragma once

#include <optional>
#include <variant>
#include <type_traits>
#include <utility>
#include <functional>

namespace lbnl
{
    //
    // Type trait to detect std::optional<T>
    //
    template<typename T>
    struct is_std_optional : std::false_type {};

    template<typename T>
    struct is_std_optional<std::optional<T>> : std::true_type {};

    //
    // Forward declaration of OptionalExt trait — full definition comes after OptionalExt
    //
    template<typename T>
    struct is_optional_ext;

    //
    // OptionalExt: Enables monadic-style chaining on std::optional<T>
    //
    template<typename T>
    class OptionalExt
    {
    public:
        explicit constexpr OptionalExt(std::optional<T> opt) : m_opt(std::move(opt)) {}

        //! Applies a function if this optional contains a value.
        //! The function may return:
        //! - a plain value,
        //! - another std::optional,
        //! - an OptionalExt,
        //! - or void.
        template<typename Func>
        [[nodiscard]] constexpr auto and_then(Func&& func) const
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
                else if constexpr (is_optional_ext<RawResult>::value)
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
                std::invoke(std::forward<Func>(func), *m_opt);
                return OptionalExt<std::monostate>(std::monostate{});
            }
            else
            {
                auto result = std::invoke(std::forward<Func>(func), *m_opt);

                if constexpr (is_std_optional<RawResult>::value)
                {
                    using Inner = typename RawResult::value_type;
                    return OptionalExt<Inner>(result);
                }
                else if constexpr (is_optional_ext<RawResult>::value)
                {
                    return result;
                }
                else
                {
                    return OptionalExt<RawResult>(result);
                }
            }
        }

        //! Returns current value if present, otherwise calls fallback function.
        //! Works with both value-returning and void-returning fallbacks.
        template<typename Func>
        [[nodiscard]] constexpr auto or_else(Func&& func) const
        {
            using Result = std::invoke_result_t<Func>;

            if constexpr (std::is_void_v<Result>)
            {
                if (!m_opt)
                {
                    std::invoke(std::forward<Func>(func));
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
                    return OptionalExt<ReturnType>(std::invoke(std::forward<Func>(func)));
                }
            }
        }

        //! Access the underlying std::optional<T>
        [[nodiscard]] constexpr const std::optional<T>& raw() const noexcept
        {
            return m_opt;
        }

    private:
        std::optional<T> m_opt;
    };

    //
    // Trait to detect lbnl::OptionalExt<T>
    //
    template<typename T>
    struct is_optional_ext : std::false_type {};

    template<typename T>
    struct is_optional_ext<lbnl::OptionalExt<T>> : std::true_type {};

    //
    // Converts a std::optional<T> into an OptionalExt<T> to enable chaining
    //
    template<typename T>
    [[nodiscard]] constexpr auto extend(const std::optional<T>& opt)
    {
        return OptionalExt<T>(opt);
    }

    //
    // Pipe operator (|) for std::optional<T>: applies transformation if value exists
    //
    template<typename T, typename Func>
    [[nodiscard]] constexpr auto operator|(const std::optional<T>& opt, Func&& func)
        -> std::optional<std::invoke_result_t<Func, const T&>>
    {
        return extend(opt).and_then(std::forward<Func>(func)).raw();
    }

    //
    // Or operator (||) for std::optional<T>: returns fallback if optional is empty
    //
    template<typename T, typename Func>
    [[nodiscard]] constexpr auto operator||(const std::optional<T>& opt, Func&& func) -> T
    {
        return opt ? *opt : std::invoke(std::forward<Func>(func));
    }

    //
    // Type trait to detect if a type T exists in a std::variant<Ts...>
    //
    template<typename T, typename Variant>
    constexpr bool is_in_variant_v = false;

    template<typename T, typename... Ts>
    constexpr bool is_in_variant_v<T, std::variant<Ts...>> = (std::is_same_v<T, Ts> || ...);

    //
    // Extracts a T from a std::variant<Ts...> as std::optional<T>
    //
    template<typename T, typename Variant>
    [[nodiscard]] constexpr std::enable_if_t<is_in_variant_v<T, Variant>, std::optional<T>>
    get_if_opt(const Variant& variant)
    {
        if (const auto* ptr = std::get_if<T>(&variant))
        {
            return *ptr;
        }
        return std::nullopt;
    }

} // namespace lbnl
