# OptionalExt - Extended Optional

The `optional.hxx` header provides `OptionalExt<T>`, an extension to `std::optional` that brings C++23-like monadic operations to C++20. It enables functional-style chaining of optional-returning operations.

## Header

```cpp
#include <lbnl/optional.hxx>
```

## Overview

| Component | Description |
|-----------|-------------|
| `OptionalExt<T>` | Extended optional class with monadic operations |
| `extend()` | Convert `std::optional<T>` to `OptionalExt<T>` |
| `get_if_opt()` | Extract type from variant as optional |

---

## OptionalExt Class

`OptionalExt<T>` wraps a `std::optional<T>` and provides monadic operations for composing nullable computations.

### Creating OptionalExt

```cpp
#include <lbnl/optional.hxx>

// From std::optional using extend()
std::optional<int> opt = 42;
auto ext = lbnl::extend(opt);

// The constructor is explicit
lbnl::OptionalExt<int> ext2(std::optional<int>(42));
```

---

## and_then

Chains an operation that may also return an optional. If the current value exists, applies the function; otherwise returns empty.

```cpp
template<typename Func>
constexpr auto and_then(Func && func) const;
```

### Example

```cpp
#include <lbnl/optional.hxx>
#include <string>

std::optional<int> parse_int(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<int> safe_divide(int x) {
    if (x == 0) return std::nullopt;
    return 100 / x;
}

int main() {
    std::optional<std::string> input = "5";

    auto result = lbnl::extend(input)
        .and_then([](const std::string& s) { return parse_int(s); })
        .and_then([](int x) { return safe_divide(x); })
        .raw();  // Get back std::optional<int>

    // result = 20 (100 / 5)
}
```

---

## or_else

Provides a fallback when the optional is empty. If the current value exists, returns it; otherwise invokes the fallback function.

```cpp
template<typename Func>
constexpr auto or_else(Func && func) const;
```

### Example

```cpp
#include <lbnl/optional.hxx>

int main() {
    std::optional<int> empty = std::nullopt;
    std::optional<int> value = 42;

    // Provide fallback value
    auto result1 = lbnl::extend(empty)
        .or_else([]() { return 0; })
        .raw();  // result1 = 0

    auto result2 = lbnl::extend(value)
        .or_else([]() { return 0; })
        .raw();  // result2 = 42

    // Side effects on empty
    lbnl::extend(empty)
        .or_else([]() {
            std::cout << "Value was empty!\n";
        });
}
```

---

## map / transform

Transforms the contained value if present. Unlike `and_then`, the function returns a plain value (not an optional). `transform` is the C++23-aligned name; `map` is an alias.

```cpp
template<typename Func>
constexpr auto map(Func && func) const;

template<typename Func>
constexpr auto transform(Func && func) const;  // C++23-aligned alias
```

### Example

```cpp
#include <lbnl/optional.hxx>
#include <string>

int main() {
    std::optional<int> value = 42;

    auto doubled = lbnl::extend(value)
        .transform([](int x) { return x * 2; })
        .raw();  // doubled = 84

    auto asString = lbnl::extend(value)
        .transform([](int x) { return std::to_string(x); })
        .raw();  // asString = "42"
}
```

---

## value_or

Returns the contained value or a fallback if empty.

```cpp
template<typename U>
[[nodiscard]] constexpr T value_or(U && fallback) const;
```

### Example

```cpp
#include <lbnl/optional.hxx>

int main() {
    std::optional<int> empty = std::nullopt;
    std::optional<int> value = 42;

    int x = lbnl::extend(empty).value_or(0);   // x = 0
    int y = lbnl::extend(value).value_or(0);   // y = 42
}
```

---

## raw

Returns the underlying `std::optional<T>`.

```cpp
[[nodiscard]] constexpr const std::optional<T> & raw() const noexcept;
```

### Example

```cpp
#include <lbnl/optional.hxx>

int main() {
    std::optional<int> opt = 42;
    auto ext = lbnl::extend(opt);

    std::optional<int> back = ext.raw();  // Convert back to std::optional
}
```

---

## Variant Helper: get_if_opt

Extracts a specific type from a `std::variant` as an optional.

```cpp
template<typename T, typename Variant>
[[nodiscard]] constexpr std::enable_if_t<is_in_variant_v<T, Variant>, std::optional<T>>
get_if_opt(const Variant & variant);
```

### Example

```cpp
#include <lbnl/optional.hxx>
#include <variant>
#include <string>

int main() {
    std::variant<int, std::string, double> v = 42;

    auto asInt = lbnl::get_if_opt<int>(v);           // std::optional<int>(42)
    auto asString = lbnl::get_if_opt<std::string>(v); // std::nullopt
    auto asDouble = lbnl::get_if_opt<double>(v);     // std::nullopt

    if (asInt) {
        std::cout << "Integer: " << *asInt << '\n';
    }
}
```

---

## Complete Example: Chaining Operations

```cpp
#include <lbnl/optional.hxx>
#include <fstream>
#include <string>

std::optional<std::string> read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) return std::nullopt;
    return std::string(std::istreambuf_iterator<char>(file), {});
}

std::optional<int> parse_config_value(const std::string& content) {
    auto pos = content.find("value=");
    if (pos == std::string::npos) return std::nullopt;
    try {
        return std::stoi(content.substr(pos + 6));
    } catch (...) {
        return std::nullopt;
    }
}

int main() {
    std::optional<std::string> configPath = "config.txt";

    int configValue = lbnl::extend(configPath)
        .and_then([](const std::string& path) { return read_file(path); })
        .and_then([](const std::string& content) { return parse_config_value(content); })
        .value_or(100);  // Default value if anything fails

    std::cout << "Config value: " << configValue << '\n';
}
```

---

## Optional Utils

The `optional_utils.hxx` header provides additional utilities for working with collections of optionals.

### average_optional

Computes the average of a vector of optional values, ignoring empty entries.

```cpp
#include <lbnl/optional_utils.hxx>

int main() {
    std::vector<std::optional<double>> values = {
        1.0, std::nullopt, 3.0, std::nullopt, 5.0
    };

    auto avg = lbnl::average_optional(values);
    // avg = 3.0 (average of 1.0, 3.0, 5.0)

    std::vector<std::optional<double>> allEmpty = {
        std::nullopt, std::nullopt
    };
    auto noAvg = lbnl::average_optional(allEmpty);
    // noAvg = std::nullopt
}
```

**Note:** For integer types, this performs integer division which truncates the result. For example, the average of `{1, 2}` returns `1`, not `1.5`. Use floating-point types if precise averaging is required.

---

## See Also

- [ExpectedExt](expected.md) - Result type for error handling
- [Algorithm Functions](algorithm.md) - Container algorithms
