# LBNLCPPCommon

A header-only C++ library for reusable utility functions

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/compiler_support)

## Overview

**LBNLCPPCommon** is a **header-only** C++ library designed to provide **commonly used functions and utilities** across multiple projects. It eliminates code duplication by offering a centralized set of **generic templates and helper functions**.

- **Header-only** - no compilation needed
- **Modern C++20 features** - concepts, ranges, `std::optional`
- **Lightweight and easy to integrate** with CMake
- **No dependencies** required beyond the standard library
- **Thread-safe** where applicable (e.g., LazyEvaluator)

## Folder Structure

```text
LBNLCPPCommon/
├── include/
│   └── lbnl/
│       ├── algorithm.hxx           # Container and range algorithms
│       ├── optional.hxx            # OptionalExt with monadic operations
│       ├── optional_utils.hxx      # Optional utility functions
│       ├── expected.hxx            # ExpectedExt for error handling
│       ├── map_utils.hxx           # Associative container utilities
│       ├── enum_index_mapper.hxx   # Bidirectional enum-index mapping
│       └── memoize.hxx             # LazyEvaluator for caching
├── docs/                           # Detailed documentation
├── tst/                            # Unit tests
├── CMakeLists.txt
├── LICENSE
└── README.md
```

## Installation

### Using FetchContent in CMake (Recommended)

```cmake
include(FetchContent)

FetchContent_Declare(
    LBNLCPPCommon
    GIT_REPOSITORY https://github.com/LBNL-ETA/LBNLCPPCommon.git
    GIT_TAG main
)

FetchContent_MakeAvailable(LBNLCPPCommon)

target_link_libraries(MyProject PRIVATE LBNLCPPCommon)
```

### Using Git Submodules

```sh
git submodule add https://github.com/LBNL-ETA/LBNLCPPCommon.git external/LBNLCPPCommon
git submodule update --init --recursive
```

Then in your `CMakeLists.txt`:

```cmake
add_subdirectory(external/LBNLCPPCommon)
target_link_libraries(MyProject PRIVATE LBNLCPPCommon)
```

## Quick Examples

### Finding Elements in Containers

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>

std::vector<int> numbers = {1, 2, 3, 4, 5};

auto found = lbnl::find_element(numbers, [](int x) { return x > 3; });
if (found) {
    std::cout << "Found: " << *found << '\n';  // Output: Found: 4
}
```

### Chaining Optional Operations

```cpp
#include <lbnl/optional.hxx>

std::optional<std::string> input = "42";

int result = lbnl::extend(input)
    .and_then([](const std::string& s) { return parse_int(s); })  // returns optional<int>
    .and_then([](int x) { return x * 2; })
    .or_else([]() { return 0; })  // fallback if any step fails
    .value_or(0);
```

### Error Handling with ExpectedExt

```cpp
#include <lbnl/expected.hxx>

using Result = lbnl::ExpectedExt<int, std::string>;

Result safe_divide(int num, int den) {
    if (den == 0) return lbnl::Unexpected(std::string("Division by zero"));
    return num / den;
}

auto result = safe_divide(10, 2)
    .and_then([](int x) { return safe_divide(x, 2); })
    .transform([](int x) { return x * 10; });

if (result.has_value()) {
    std::cout << result.value() << '\n';  // 25
}
```

### Caching Expensive Computations

```cpp
#include <lbnl/memoize.hxx>

lbnl::LazyEvaluator<std::string, std::string> fileCache(
    [](const std::string& path) {
        return read_file_content(path);  // expensive operation
    }
);

const auto & content1 = fileCache("config.txt");  // reads file
const auto & content2 = fileCache("config.txt");  // returns cached reference
```

## Library Components

### Algorithm Functions ([docs/algorithm.md](docs/algorithm.md))

Generic algorithms for containers and ranges.

| Function | Description |
|----------|-------------|
| `find_element` | Find first element matching a predicate |
| `contains` | Check if container contains a value |
| `sorted_unique` | Remove duplicates from a range (sorts first) |
| `zip` | Combine two ranges into pairs |
| `filter` | Filter elements by predicate |
| `transform_if` | Transform matching elements, copy others |
| `transform_filter` | Transform and filter in one pass |
| `merge` | Merge two sorted ranges |
| `split` | Split string by delimiter |
| `partition` | Split range into two groups |
| `flatten` | Flatten nested vectors |
| `to_vector` | Convert any range to vector |
| `transform_to_vector` | Transform range to vector |

### OptionalExt ([docs/optional.md](docs/optional.md))

Extended optional with C++23-like monadic operations.

| Method | Description |
|--------|-------------|
| `and_then` | Chain optional-returning operations |
| `or_else` | Provide fallback for empty optional |
| `map` / `transform` | Transform contained value |
| `value_or` | Get value or fallback |

Also includes `average_optional()` for computing averages of optional vectors.

### ExpectedExt ([docs/expected.md](docs/expected.md))

Result type for error handling without exceptions.

| Method | Description |
|--------|-------------|
| `has_value` | Check if result is success |
| `value` | Get success value |
| `error` | Get error value |
| `value_or` | Get value or fallback |
| `and_then` | Chain on success (func returns `ExpectedExt`) |
| `or_else` | Handle errors |
| `transform` | Transform success value (func returns plain value) |
| `transform_error` | Transform error value |

### Map Utilities ([docs/map_utils.md](docs/map_utils.md))

Utilities for associative containers (`std::map`, `std::unordered_map`).

| Function | Description |
|----------|-------------|
| `map_lookup_by_key` | Safe key lookup returning optional |
| `map_lookup_by_value` | Reverse lookup by value |
| `map_keys` | Extract all keys as vector |
| `map_values` | Extract all values as vector |

### EnumIndexMapper ([docs/enum_index_mapper.md](docs/enum_index_mapper.md))

Bidirectional mapping between enum values and database indices.

| Method | Description |
|--------|-------------|
| `to_enum` | Convert index to enum |
| `to_index` | Convert enum to index |
| `to_enum_or` | Convert with fallback enum |
| `to_index_or` | Convert with fallback index |

### LazyEvaluator ([docs/memoize.md](docs/memoize.md))

Thread-safe memoization for expensive computations.

| Method | Description |
|--------|-------------|
| `operator()` | Get or compute value (returns `const Value &`) |
| `get` | Get or compute value (returns `const Value &`) |

## Documentation

Detailed documentation for each component is available in the `docs/` folder:

- [Algorithm Functions](docs/algorithm.md)
- [OptionalExt](docs/optional.md)
- [ExpectedExt](docs/expected.md)
- [Map Utilities](docs/map_utils.md)
- [EnumIndexMapper](docs/enum_index_mapper.md)
- [LazyEvaluator (Memoize)](docs/memoize.md)

## Requirements

- C++20 compatible compiler
- CMake 3.14+

## License

This project is licensed under the Berkeley Lab License - see the [LICENSE](LICENSE) file for details.

## Contributing

We welcome contributions! Feel free to open issues, suggest improvements, or submit pull requests.

## Contact

For any questions or suggestions, please reach out to the **LBNL-ETA** team.
