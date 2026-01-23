# ExpectedExt - Result Type for Error Handling

The `expected.hxx` header provides `ExpectedExt<T, E>`, a result type that can hold either a success value (`T`) or an error (`E`). This is similar to C++23's `std::expected` and provides a way to handle errors without exceptions.

## Header

```cpp
#include <lbnl/expected.hxx>
```

## Overview

| Component | Description |
|-----------|-------------|
| `ExpectedExt<T, E>` | Result type holding value or error |
| `make_expected<T, E>()` | Create a success result |
| `make_unexpected<T, E>()` | Create an error result |
| `operator\|` | Pipe operator for chaining |

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

// Error value
lbnl::ExpectedExt<int, std::string> failure(std::string("Something went wrong"));

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

Returns the contained value. Throws if the result is an error.

```cpp
[[nodiscard]] constexpr const T & value() const;
[[nodiscard]] constexpr T & value();
```

### error

Returns the contained error. Throws if the result is a value.

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
    lbnl::ExpectedExt<int, std::string> failure(std::string("Error"));

    int val1 = success.value();        // 42
    std::string err = failure.error(); // "Error"

    int val2 = failure.value_or(0);    // 0 (fallback)
    int val3 = success.value_or(0);    // 42
}
```

---

## and_then

Chains an operation if the result is a success. The function can return either another `ExpectedExt<U, E>` or a plain value `U`.

```cpp
template<typename Func>
[[nodiscard]] constexpr auto and_then(Func && func) const;
```

### Example

```cpp
#include <lbnl/expected.hxx>
#include <string>

using Result = lbnl::ExpectedExt<int, std::string>;

Result safe_divide(int a, int b) {
    if (b == 0) {
        return Result(std::string("Division by zero"));
    }
    return Result(a / b);
}

Result safe_sqrt(int x) {
    if (x < 0) {
        return Result(std::string("Cannot take sqrt of negative"));
    }
    return Result(static_cast<int>(std::sqrt(x)));
}

int main() {
    Result start(100);

    auto result = start
        .and_then([](int x) { return safe_divide(x, 5); })   // 100 / 5 = 20
        .and_then([](int x) { return safe_divide(x, 2); })   // 20 / 2 = 10
        .and_then([](int x) { return safe_sqrt(x); });       // sqrt(10) = 3

    if (result) {
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
    Result failure(std::string("Initial error"));

    auto recovered = failure.or_else([](const std::string& err) {
        std::cout << "Handling error: " << err << '\n';
        return Result(0);  // Recover with default value
    });

    std::cout << "Recovered value: " << recovered.value() << '\n';  // 0
}
```

---

## map

Transforms the success value. Unlike `and_then`, the function returns a plain value (not an `ExpectedExt`).

```cpp
template<typename Func>
[[nodiscard]] constexpr auto map(Func && func) const;
```

### Example

```cpp
#include <lbnl/expected.hxx>
#include <string>

int main() {
    lbnl::ExpectedExt<int, std::string> result(42);

    auto doubled = result.map([](int x) { return x * 2; });
    // doubled = ExpectedExt<int, std::string>(84)

    auto asString = result.map([](int x) { return std::to_string(x); });
    // asString = ExpectedExt<std::string, std::string>("42")
}
```

---

## map_error

Transforms the error value while preserving the success value.

```cpp
template<typename Func>
[[nodiscard]] constexpr auto map_error(Func && func) const;
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
    lbnl::ExpectedExt<int, std::string> failure(std::string("Not found"));

    auto withCode = failure.map_error([](const std::string& msg) {
        return DetailedError{msg, 404};
    });
    // withCode = ExpectedExt<int, DetailedError>

    if (!withCode) {
        std::cout << "Error: " << withCode.error().message
                  << " (code " << withCode.error().code << ")\n";
    }
}
```

---

## Pipe Operator

The `operator|` provides a convenient syntax for chaining `and_then` operations.

```cpp
template<typename T, typename E, typename Func>
[[nodiscard]] constexpr auto operator|(const ExpectedExt<T, E> & exp, Func && func);
```

### Example

```cpp
#include <lbnl/expected.hxx>
#include <string>

using Result = lbnl::ExpectedExt<int, std::string>;

Result parse_int(const std::string& s) {
    try {
        return Result(std::stoi(s));
    } catch (...) {
        return Result(std::string("Parse error"));
    }
}

int main() {
    Result input(42);

    auto result = input
        | [](int x) { return x * 2; }
        | [](int x) { return std::to_string(x); };
    // result = ExpectedExt<std::string, std::string>("84")
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
        return FileResult(std::string("Failed to open file: " + path));
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return FileResult(buffer.str());
}

ParseResult parse_number(const std::string& content) {
    try {
        return ParseResult(std::stoi(content));
    } catch (const std::exception& e) {
        return ParseResult(std::string("Parse error: ") + e.what());
    }
}

ParseResult validate_positive(int value) {
    if (value <= 0) {
        return ParseResult(std::string("Value must be positive"));
    }
    return ParseResult(value);
}

int main() {
    auto result = read_file("input.txt")
        .and_then([](const std::string& content) { return parse_number(content); })
        .and_then([](int value) { return validate_positive(value); })
        .map([](int value) { return value * 2; });

    if (result) {
        std::cout << "Processed value: " << result.value() << '\n';
    } else {
        std::cout << "Error: " << result.error() << '\n';
    }

    // Or use value_or for default
    int finalValue = result.value_or(-1);
}
```

---

## Comparison with Exceptions

| Aspect | ExpectedExt | Exceptions |
|--------|-------------|------------|
| Error visibility | Explicit in return type | Hidden in call stack |
| Performance | No stack unwinding | May have overhead |
| Composability | Easy to chain | Try-catch blocks |
| Forgetting to handle | Compile-time visible | Runtime surprise |

---

## See Also

- [OptionalExt](optional.md) - For nullable values without error information
- [Algorithm Functions](algorithm.md) - Container algorithms
