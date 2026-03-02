// cpp
#pragma once

#include <variant>
#include <type_traits>
#include <functional>
#include <utility>

namespace lbnl
{

    //
    // Unexpected<E>: wrapper to disambiguate error values from success values
    // Similar to C++23's std::unexpected
    //
    template<typename E>
    struct Unexpected
    {
        E error;

        constexpr explicit Unexpected(E e) : error(std::move(e))
        {}
    };

    // Deduction guide for Unexpected
    template<typename E>
    Unexpected(E) -> Unexpected<E>;

    // Forward declaration
    template<typename T, typename E>
    class ExpectedExt;

    //
    // Trait to detect if something is an ExpectedExt
    //
    template<typename T>
    struct is_expected_ext : std::false_type
    {};

    template<typename T, typename E>
    struct is_expected_ext<ExpectedExt<T, E>> : std::true_type
    {};

    //
    // ExpectedExt<T, E>: like std::expected<T, E> in C++23
    //
    template<typename T, typename E>
    class ExpectedExt
    {
    public:
        using value_type = T;
        using error_type = E;

        // defaulted special members
        constexpr ExpectedExt() = delete;
        constexpr ExpectedExt(const ExpectedExt &) = default;
        constexpr ExpectedExt(ExpectedExt &&) = default;
        constexpr ExpectedExt & operator=(const ExpectedExt &) = default;
        constexpr ExpectedExt & operator=(ExpectedExt &&) = default;
        ~ExpectedExt() = default;

        // Construct from value (implicit)
        constexpr ExpectedExt(T value) : m_data(std::in_place_index<0>, std::move(value)), m_HasValue(true)
        {}

        // Construct from Unexpected wrapper (implicit) - disambiguates errors
        constexpr ExpectedExt(Unexpected<E> err) : m_data(std::in_place_index<1>, std::move(err.error)), m_HasValue(false)
        {}

        [[nodiscard]] constexpr bool has_value() const noexcept
        {
            return m_HasValue;
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return has_value();
        }

        [[nodiscard]] constexpr const T & value() const
        {
            return std::get<0>(m_data);
        }

        [[nodiscard]] constexpr T & value()
        {
            return std::get<0>(m_data);
        }

        [[nodiscard]] constexpr const E & error() const
        {
            return std::get<1>(m_data);
        }

        [[nodiscard]] constexpr E & error()
        {
            return std::get<1>(m_data);
        }

        //! Returns the contained value or the provided alternative
        template<typename U>
        [[nodiscard]] constexpr T value_or(U && alt) const
        {
            return has_value() ? value() : static_cast<T>(std::forward<U>(alt));
        }

        //! Applies a function to the value if present.
        //! Supports chaining with:
        //! - another ExpectedExt<U, E>
        //! - a plain value U
        template<typename Func>
        [[nodiscard]] constexpr auto and_then(Func && func) const
        {
            using Raw = std::invoke_result_t<Func, const T &>;

            if constexpr(is_expected_ext<Raw>::value)
            {
                using U = typename Raw::value_type;
                using Ret = ExpectedExt<U, E>;
                if(!has_value())
                {
                    return Ret(Unexpected<E>(error()));
                }
                return std::invoke(std::forward<Func>(func), value());
            }
            else
            {
                using Ret = ExpectedExt<Raw, E>;
                if(!has_value())
                {
                    return Ret(Unexpected<E>(error()));
                }
                return Ret(std::invoke(std::forward<Func>(func), value()));
            }
        }

        //! Fallback handler if there's an error
        template<typename Func>
        [[nodiscard]] constexpr auto or_else(Func && func) const
        {
            if(has_value())
            {
                return *this;
            }
            else
            {
                return std::invoke(std::forward<Func>(func), error());
            }
        }

        template<typename Func>
        [[nodiscard]] constexpr auto map(Func && func) const
        {
            using U = std::remove_cvref_t<std::invoke_result_t<Func, const T &>>;

            if(has_value())
            {
                return ExpectedExt<U, E>(std::invoke(std::forward<Func>(func), value()));
            }
            else
            {
                return ExpectedExt<U, E>(Unexpected<E>(error()));
            }
        }

        template<typename Func>
        [[nodiscard]] constexpr auto map_error(Func && func) const
        {
            using E2 = std::remove_cvref_t<std::invoke_result_t<Func, const E &>>;

            if(has_value())
            {
                return ExpectedExt<T, E2>(value());
            }
            else
            {
                return ExpectedExt<T, E2>(Unexpected<E2>(std::invoke(std::forward<Func>(func), error())));
            }
        }

    private:
        std::variant<T, E> m_data;
        bool m_HasValue;
    };

    //
    // Convenience constructors
    //
    template<typename T, typename E>
    [[nodiscard]] constexpr ExpectedExt<T, E> make_expected(T value)
    {
        return ExpectedExt<T, E>(std::move(value));
    }

    template<typename T, typename E>
    [[nodiscard]] constexpr ExpectedExt<T, E> make_unexpected(E error)
    {
        return ExpectedExt<T, E>(Unexpected<E>(std::move(error)));
    }

}   // namespace lbnl
