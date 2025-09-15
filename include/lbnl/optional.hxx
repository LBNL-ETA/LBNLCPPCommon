// optional.hxx
#pragma once
#include <optional>
#include <variant>
#include <type_traits>
#include <utility>
#include <functional>

namespace lbnl
{

    // Detect std::optional<T>
    template<typename T>
    struct is_std_optional : std::false_type
    {};
    template<typename T>
    struct is_std_optional<std::optional<T>> : std::true_type
    {};

    // Primary template; specialization appears later
    template<typename T>
    struct is_optional_ext;

    // Forward-declare the class so we can mention it in helper decls
    template<typename U>
    class OptionalExt;

    // ---- declare helpers (no bodies!) so and_then can call them
    namespace detail
    {
        template<class R>
        constexpr auto to_ext_empty();

        template<class R>
        constexpr auto to_ext_from(R && r);
    }   // namespace detail

    // ---------------- OptionalExt ----------------
    template<typename T>
    class OptionalExt
    {
    public:
        explicit constexpr OptionalExt(std::optional<T> opt) : m_opt(std::move(opt))
        {}

        template<typename Func>
        constexpr auto and_then(Func && func) const
        {
            using Raw = std::invoke_result_t<Func, const T &>;
            using RR = std::remove_cvref_t<Raw>;

            if(!m_opt)
            {
                return detail::to_ext_empty<RR>();
            }

            if constexpr(std::is_void_v<RR>)
            {
                std::invoke(std::forward<Func>(func), *m_opt);
                return OptionalExt<std::monostate>(std::monostate{});
            }
            else
            {
                auto r = std::invoke(std::forward<Func>(func), *m_opt);
                return detail::to_ext_from(std::move(r));
            }
        }

        template<typename Func>
        constexpr auto or_else(Func && func) const
        {
            using Result = std::invoke_result_t<Func>;

            if constexpr(std::is_void_v<Result>)
            {
                if(!m_opt)
                {
                    std::invoke(std::forward<Func>(func));
                }
                return OptionalExt<std::monostate>(std::monostate{});
            }
            else
            {
                using ReturnType = std::decay_t<Result>;
                if(m_opt)
                {
                    return OptionalExt<ReturnType>(*m_opt);
                }
                else
                {
                    return OptionalExt<ReturnType>(std::invoke(std::forward<Func>(func)));
                }
            }
        }

        template<typename U>
        [[nodiscard]] constexpr T value_or(U && fallback) const
        {
            return m_opt.value_or(std::forward<U>(fallback));
        }

        template<typename Func>
        constexpr auto map(Func && func) const
        {
            using U = std::invoke_result_t<Func, const T &>;
            if(m_opt)
            {
                return OptionalExt<U>(std::invoke(std::forward<Func>(func), *m_opt));
            }
            else
            {
                return OptionalExt<U>(std::nullopt);
            }
        }

        template<typename Func>
        constexpr auto transform(Func && func) const
        {
            return map(std::forward<Func>(func));
        }

        [[nodiscard]] constexpr const std::optional<T> & raw() const noexcept
        {
            return m_opt;
        }

    private:
        std::optional<T> m_opt;
    };

    // Trait to detect lbnl::OptionalExt<T>
    template<typename T>
    struct is_optional_ext : std::false_type
    {};
    template<typename T>
    struct is_optional_ext<lbnl::OptionalExt<T>> : std::true_type
    {};

    // ---- define helpers *after* class & specialization
    namespace detail
    {

        template<class R>
        constexpr auto to_ext_empty()
        {
            using RR = std::remove_cvref_t<R>;
            if constexpr(std::is_void_v<RR>)
            {
                return OptionalExt<std::monostate>(std::nullopt);
            }
            else if constexpr(is_std_optional<RR>::value)
            {
                using U = typename RR::value_type;
                return OptionalExt<U>(std::nullopt);
            }
            else if constexpr(is_optional_ext<RR>::value)
            {
                using U = typename RR::value_type;
                return OptionalExt<U>(std::nullopt);
            }
            else
            {
                using U = RR;
                return OptionalExt<U>(std::nullopt);
            }
        }

        template<class R>
        constexpr auto to_ext_from(R && r)
        {
            using RR = std::remove_cvref_t<R>;
            if constexpr(is_std_optional<RR>::value)
            {
                using U = typename RR::value_type;
                return OptionalExt<U>(std::forward<R>(r));
            }
            else if constexpr(is_optional_ext<RR>::value)
            {
                return std::forward<R>(r);   // already OptionalExt<U>
            }
            else
            {
                using U = RR;
                return OptionalExt<U>(std::forward<R>(r));
            }
        }

    }   // namespace detail

    // Adapter and pipe operators
    template<typename T>
    [[nodiscard]] constexpr auto extend(const std::optional<T> & opt)
    {
        return OptionalExt<T>(opt);
    }

    template<typename T, typename Func>
    [[nodiscard]] constexpr auto operator|(const std::optional<T> & opt, Func && func)
    {
        return extend(opt).and_then(std::forward<Func>(func)).raw();
    }

    template<typename T, typename Func>
    [[nodiscard]] constexpr T operator||(const std::optional<T> & opt, Func && func)
    {
        return opt ? *opt : std::invoke(std::forward<Func>(func));
    }

    // Variant helper
    template<typename T, typename Variant>
    constexpr bool is_in_variant_v = false;

    template<typename T, typename... Ts>
    constexpr bool is_in_variant_v<T, std::variant<Ts...>> = (std::is_same_v<T, Ts> || ...);

    template<typename T, typename Variant>
    [[nodiscard]] constexpr std::enable_if_t<is_in_variant_v<T, Variant>, std::optional<T>>
      get_if_opt(const Variant & variant)
    {
        if(const auto * ptr = std::get_if<T>(&variant))
            return *ptr;
        return std::nullopt;
    }

}   // namespace lbnl
