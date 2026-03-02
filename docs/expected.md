# ExpectedExt - Result Type for Error Handling

The `expected.hxx` header provides `ExpectedExt<T, E>`, a result type that can hold either a success value (`T`) or an error (`E`). This is designed to match C++23's `std::expected` API so that migration is straightforward.

## Header

```cpp
#include <lbnl/expected.hxx>
```

## Overview

| Component | Description |
|-----------|-------------|
| `ExpectedExt<T, E>` | Result type holding value or error |
| `Unexpected<E>` | Wrapper to disambiguate error values from success values |
| `make_expected<T, E>()` | Create a success result |
| `make_unexpected<T, E>()` | Create an error result |

---

## Unexpected Wrapper

`Unexpected<E>` is a wrapper struct that explicitly marks a value as an error. This is particularly useful when `T` and `E` are the same type (e.g., both are `std::string`), as it disambiguates whether the value is a success or an error.

```cpp
template<typename E>
struct Unexpected {
    E error;
    constexpr explicit Unexpected(E e);
};

// Deduction guide allows: Unexpected(myError) without specifying template
template<typename E>
Unexpected(E) -> Unexpected<E>;
```

### Example

```cpp
#include <lbnl/expected.hxx>
#include <string>

// When T and E are both std::string, use Unexpected to clarify intent
using Result = lbnl::ExpectedExt<std::string, std::string>;

Result get_greeting(bool success) {
    if (success) {
        return Result("Hello!");  // This is the success value
    }
    return lbnl::Unexpected(std::string("Failed to generate greeting"));  // This is the error
}
```

---

## ExpectedExt Class

`ExpectedExt<T, E>` represents the result of an operation that can either succeed with a value of type `T` or fail with an error of type `E`.

### Type Aliases

```cpp
using value_type = T;
using error_type = E;
```

### Creating ExpectedExt

```cpp
#include <lbnl/expected.hxx>
#include <string>

// Success value
lbnl::ExpectedExt<int, std::string> success(42);

// Error value using Unexpected wrapper
lbnl::ExpectedExt<int, std::string> failure(lbnl::Unexpected(std::string("Something went wrong")));

// Using helper functions
auto ok = lbnl::make_expected<int, std::string>(42);
auto err = lbnl::make_unexpected<int, std::string>("Error message");
```

---

## Checking State

### has_value / operator bool

```cpp
[[nodiscard]] constexpr bool has_value() const noexcept;
[[nodiscard]] constexpr explicit operator bool() const noexcept;
```

### Example

```cpp
#include <lbnl/expected.hxx>

int main() {
    lbnl::ExpectedExt<int, std::string> result(42);

    if (result.has_value()) {
        std::cout << "Success!\n";
    }

    // Or using bool conversion
    if (result) {
        std::cout << "Also success!\n";
    }
}
```

---

## Accessing Values

### value

Returns the contained value. Behavior is undefined if the result is an error.

```cpp
[[nodiscard]] constexpr const T & value() const;
[[nodiscard]] constexpr T & value();
```

### error

Returns the contained error. Behavior is undefined if the result is a value.

```cpp
[[nodiscard]] constexpr const E & error() const;
[[nodiscard]] constexpr E & error();
```

### value_or

Returns the contained value or a fallback if it's an error.

```cpp
template<typename U>
[[nodiscard]] constexpr T value_or(U && alt) const;
```

### Example

```cpp
#include <lbnl/expected.hxx>

int main() {
    lbnl::ExpectedExt<int, std::string> success(42);
    auto failure = lbnl::make_unexpected<int, std::string>("Error");

    int val1 = success.value();        // 42
    std::string err = failure.error(); // "Error"

    int val2 = failure.value_or(0);    // 0 (fallback)
    int val3 = success.value_or(0);    // 42
}
```

---

## and_then

Chains an operation on success. The function **must** return `ExpectedExt<U, E>`.
For plain-value transforms, use `transform()` instead.

This matches C++23 `std::expected::and_then` semantics.

```cpp
template<typename Func>
[[nodiscard]] constexpr auto and_then(Func && func) const;
```

### Example

```cpp
#include <lbnl/expected.hxx>
#include <string>

using Result = lbnl::ExpectedExt<int, std::string>;

Result safe_divide(int num, int den) {
    if (den == 0) return lbnl::Unexpected(std::string("Division by zero"));
    return num / den;
}

Result safe_sqrt(int x) {
    if (x < 0) return lbnl::Unexpected(std::string("Cannot take sqrt of negative"));
    return static_cast<int>(std::sqrt(x));
}

int main() {
    Result start(100);

    auto result = start
        .and_then([](int x) { return safe_divide(x, 5); })   // 100 / 5 = 20
        .and_then([](int x) { return safe_divide(x, 2); })   // 20 / 2 = 10
        .and_then([](int x) { return safe_sqrt(x); });       // sqrt(10) = 3

    if (result.has_value()) {
        std::cout << "Result: " << result.value() << '\n';  // 3
    }
}
```

---

## or_else

Provides error recovery. If the result is an error, invokes the handler which should return an `ExpectedExt<T, E>`.

```cpp
template<typename Func>
[[nodiscard]] constexpr auto or_else(Func && func) const;
```

### Example

```cpp
#include <lbnl/expected.hxx>
#include <string>

using Result = lbnl::ExpectedExt<int, std::string>;

int main() {
    auto failure = lbnl::make_unexpected<int, std::string>("Initial error");

    auto recovered = failure.or_else([](const std::string& err) {
        std::cout << "Handling error: " << err << '\n';
        return Result(0);  // Recover with default value
    });

    std::cout << "Recovered value: " << recovered.value() << '\n';  // 0
}
```

---

## transform

Transforms the success value. The function returns a plain value (not an `ExpectedExt`).
Matches C++23 `std::expected::transform`.

```cpp
template<typename Func>
[[nodiscard]] constexpr auto transform(Func && func) const;
```

### Example

```cpp
#include <lbnl/expected.hxx>
#include <string>

int main() {
    lbnl::ExpectedExt<int, std::string> result(42);

    auto doubled = result.transform([](int x) { return x * 2; });
    // doubled = ExpectedExt<int, std::string>(84)

    auto asString = result.transform([](int x) { return std::to_string(x); });
    // asString = ExpectedExt<std::string, std::string>("42")
}
```

---

## transform_error

Transforms the error value while preserving the success value.
Matches C++23 `std::expected::transform_error`.

```cpp
template<typename Func>
[[nodiscard]] constexpr auto transform_error(Func && func) const;
```

### Example

```cpp
#include <lbnl/expected.hxx>
#include <string>

struct DetailedError {
    std::string message;
    int code;
};

int main() {
    auto failure = lbnl::make_unexpected<int, std::string>("Not found");

    auto withCode = failure.transform_error([](const std::string& msg) {
        return DetailedError{msg, 404};
    });
    // withCode = ExpectedExt<int, DetailedError>

    if (!withCode.has_value()) {
        std::cout << "Error: " << withCode.error().message
                  << " (code " << withCode.error().code << ")\n";
    }
}
```

---

## Complete Example: File Processing Pipeline

```cpp
#include <lbnl/expected.hxx>
#include <fstream>
#include <string>
#include <sstream>

using FileResult = lbnl::ExpectedExt<std::string, std::string>;
using ParseResult = lbnl::ExpectedExt<int, std::string>;

FileResult read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        return lbnl::Unexpected(std::string("Failed to open file: " + path));
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

ParseResult parse_number(const std::string& content) {
    try {
        return std::stoi(content);
    } catch (const std::exception& exc) {
        return lbnl::Unexpected(std::string("Parse error: ") + exc.what());
    }
}

ParseResult validate_positive(int value) {
    if (value <= 0) {
        return lbnl::Unexpected(std::string("Value must be positive"));
    }
    return value;
}

int main() {
    auto result = read_file("input.txt")
        .and_then([](const std::string& content) { return parse_number(content); })
        .and_then([](int value) { return validate_positive(value); })
        .transform([](int value) { return value * 2; });

    if (result.has_value()) {
        std::cout << "Processed value: " << result.value() << '\n';
    } else {
        std::cout << "Error: " << result.error() << '\n';
    }

    // Or use value_or for default
    int finalValue = result.value_or(-1);
}
```

---

## C++23 Migration Guide

When migrating to C++23's `std::expected`, the API maps directly:

| LBNLCPPCommon | C++23 `std::expected` |
|---------------|----------------------|
| `ExpectedExt<T, E>` | `std::expected<T, E>` |
| `Unexpected<E>` | `std::unexpected<E>` |
| `and_then` | `and_then` |
| `or_else` | `or_else` |
| `transform` | `transform` |
| `transform_error` | `transform_error` |
| `has_value` | `has_value` |
| `value` / `error` | `value` / `error` |
| `value_or` | `value_or` |

---

## See Also

- [OptionalExt](optional.md) - For nullable values without error information
- [Algorithm Functions](algorithm.md) - Container algorithms
