#pragma once

#include <variant>
#include <type_traits>
#include <functional>
#include <utility>

namespace lbnl {

    // Forward declaration
    template<typename T, typename E>
    class ExpectedExt;

    //
    // Trait to detect if something is an ExpectedExt
    //
    template<typename T>
    struct is_expected_ext : std::false_type {};

    template<typename T, typename E>
    struct is_expected_ext<ExpectedExt<T, E>> : std::true_type {};

    //
    // ExpectedExt<T, E>: like std::expected<T, E> in C++23
    //
    template<typename T, typename E>
    class ExpectedExt {
    public:
        using value_type = T;
        using error_type = E;

        constexpr ExpectedExt(T value) : m_data(std::move(value)) {}
        constexpr ExpectedExt(E error) : m_data(std::move(error)) {}

        [[nodiscard]] constexpr bool has_value() const noexcept {
            return std::holds_alternative<T>(m_data);
        }

        [[nodiscard]] constexpr const T& value() const {
            return std::get<T>(m_data);
        }

        [[nodiscard]] constexpr T& value() {
            return std::get<T>(m_data);
        }

        [[nodiscard]] constexpr const E& error() const {
            return std::get<E>(m_data);
        }

        [[nodiscard]] constexpr E& error() {
            return std::get<E>(m_data);
        }

        //! Applies a function to the value if present.
        //! Supports chaining with:
        //! - another ExpectedExt<U, E>
        //! - a plain value U
        template<typename Func>
        [[nodiscard]] constexpr auto and_then(Func&& func) const {
            using RawResult = std::invoke_result_t<Func, const T&>;

            if (!has_value()) {
                return ExpectedExt<T, E>(error());
            }

            if constexpr (is_expected_ext<RawResult>::value) {
                return std::invoke(std::forward<Func>(func), value());
            } else {
                return ExpectedExt<RawResult, E>(std::invoke(std::forward<Func>(func), value()));
            }
        }

        //! Fallback handler if there's an error
        template<typename Func>
        [[nodiscard]] constexpr auto or_else(Func&& func) const {
            if (has_value()) {
                return *this;
            } else {
                return std::invoke(std::forward<Func>(func), error());
            }
        }

    private:
        std::variant<T, E> m_data;
    };

    //
    // Convenience constructors
    //
    template<typename T, typename E>
    [[nodiscard]] constexpr ExpectedExt<T, E> make_expected(T value) {
        return ExpectedExt<T, E>(std::move(value));
    }

    template<typename T, typename E>
    [[nodiscard]] constexpr ExpectedExt<T, E> make_unexpected(E error) {
        return ExpectedExt<T, E>(std::move(error));
    }

    //
    // Pipe operator (|) for ExpectedExt
    //
    template<typename T, typename E, typename Func>
    [[nodiscard]] constexpr auto operator|(const ExpectedExt<T, E>& exp, Func&& func) {
        return exp.and_then(std::forward<Func>(func));
    }

} // namespace lbnl