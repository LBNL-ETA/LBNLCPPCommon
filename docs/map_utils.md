# Map Utilities

The `map_utils.hxx` header provides utility functions for working with associative containers like `std::map` and `std::unordered_map`. All functions use C++20 concepts for type safety.

## Header

```cpp
#include <lbnl/map_utils.hxx>
```

## Overview

| Function | Description |
|----------|-------------|
| `map_lookup_by_key` | Find value by key, returns optional |
| `map_lookup_by_value` | Find key by value, returns optional |
| `map_keys` | Extract all keys as a vector |
| `map_values` | Extract all values as a vector |

---

## AssociativeContainer Concept

The library defines a concept that constrains functions to work only with map-like containers.

```cpp
template<typename Map>
concept AssociativeContainer =
    requires(const Map m, typename Map::key_type key) {
        { m.find(key) } -> std::same_as<typename Map::const_iterator>;
        { m.begin() } -> std::same_as<typename Map::const_iterator>;
        { m.end() } -> std::same_as<typename Map::const_iterator>;
    };
```

This concept is satisfied by:
- `std::map<K, V>`
- `std::unordered_map<K, V>`
- `std::multimap<K, V>`
- `std::unordered_multimap<K, V>`
- Any custom container with similar interface

---

## map_lookup_by_key

Looks up a value by its key, returning an optional.

```cpp
template<AssociativeContainer Map>
[[nodiscard]] constexpr auto map_lookup_by_key(const Map& m, const typename Map::key_type& key)
    -> std::optional<typename Map::mapped_type>;
```

### Parameters

- `m` - The map to search
- `key` - The key to look up

### Returns

`std::optional<mapped_type>` containing the value if found, or `std::nullopt` if the key doesn't exist.

### Example

```cpp
#include <lbnl/map_utils.hxx>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> ages = {
        {"Alice", 25},
        {"Bob", 30},
        {"Charlie", 35}
    };

    auto aliceAge = lbnl::map_lookup_by_key(ages, std::string("Alice"));
    if (aliceAge) {
        std::cout << "Alice is " << *aliceAge << " years old\n";  // 25
    }

    auto unknownAge = lbnl::map_lookup_by_key(ages, std::string("David"));
    if (!unknownAge) {
        std::cout << "David not found\n";
    }
}
```

### With unordered_map

```cpp
#include <lbnl/map_utils.hxx>
#include <unordered_map>

int main() {
    std::unordered_map<int, std::string> codes = {
        {200, "OK"},
        {404, "Not Found"},
        {500, "Internal Server Error"}
    };

    auto status = lbnl::map_lookup_by_key(codes, 404);
    // status = std::optional<std::string>("Not Found")
}
```

---

## map_lookup_by_value

Looks up a key by its value, returning an optional. Performs a linear search through the map.

```cpp
template<AssociativeContainer Map>
[[nodiscard]] constexpr auto map_lookup_by_value(const Map& m, const typename Map::mapped_type& value)
    -> std::optional<typename Map::key_type>;
```

### Parameters

- `m` - The map to search
- `value` - The value to look up

### Returns

`std::optional<key_type>` containing the first key with the matching value, or `std::nullopt` if not found.

### Example

```cpp
#include <lbnl/map_utils.hxx>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> ages = {
        {"Alice", 25},
        {"Bob", 30},
        {"Charlie", 35}
    };

    // Find who is 30 years old
    auto person = lbnl::map_lookup_by_value(ages, 30);
    if (person) {
        std::cout << *person << " is 30 years old\n";  // Bob
    }

    // Find age that doesn't exist
    auto nobody = lbnl::map_lookup_by_value(ages, 100);
    // nobody = std::nullopt
}
```

### Note on Multiple Matches

If multiple keys have the same value, `map_lookup_by_value` returns the first one encountered (based on iterator order).

```cpp
std::map<std::string, int> scores = {
    {"Alice", 100},
    {"Bob", 100},
    {"Charlie", 90}
};

auto winner = lbnl::map_lookup_by_value(scores, 100);
// Returns "Alice" (first in sorted order for std::map)
```

---

## map_keys

Extracts all keys from the map as a vector.

```cpp
template<AssociativeContainer Map>
[[nodiscard]] constexpr auto map_keys(const Map& m) -> std::vector<typename Map::key_type>;
```

### Parameters

- `m` - The map to extract keys from

### Returns

A `std::vector<key_type>` containing all keys.

### Example

```cpp
#include <lbnl/map_utils.hxx>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> ages = {
        {"Alice", 25},
        {"Bob", 30},
        {"Charlie", 35}
    };

    auto names = lbnl::map_keys(ages);
    // names = {"Alice", "Bob", "Charlie"}

    for (const auto& name : names) {
        std::cout << name << '\n';
    }
}
```

---

## map_values

Extracts all values from the map as a vector.

```cpp
template<AssociativeContainer Map>
[[nodiscard]] constexpr auto map_values(const Map& m) -> std::vector<typename Map::mapped_type>;
```

### Parameters

- `m` - The map to extract values from

### Returns

A `std::vector<mapped_type>` containing all values.

### Example

```cpp
#include <lbnl/map_utils.hxx>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> ages = {
        {"Alice", 25},
        {"Bob", 30},
        {"Charlie", 35}
    };

    auto allAges = lbnl::map_values(ages);
    // allAges = {25, 30, 35}

    // Calculate average age
    int sum = 0;
    for (int age : allAges) {
        sum += age;
    }
    double average = static_cast<double>(sum) / allAges.size();
}
```

---

## Complete Example: Configuration Management

```cpp
#include <lbnl/map_utils.hxx>
#include <lbnl/optional_pipe_import.hxx>
#include <map>
#include <string>
#include <iostream>

class Config {
public:
    void set(const std::string& key, const std::string& value) {
        settings_[key] = value;
    }

    std::optional<std::string> get(const std::string& key) const {
        return lbnl::map_lookup_by_key(settings_, key);
    }

    std::optional<std::string> find_key_for_value(const std::string& value) const {
        return lbnl::map_lookup_by_value(settings_, value);
    }

    std::vector<std::string> all_keys() const {
        return lbnl::map_keys(settings_);
    }

    void print_all() const {
        auto keys = lbnl::map_keys(settings_);
        auto values = lbnl::map_values(settings_);

        for (size_t i = 0; i < keys.size(); ++i) {
            std::cout << keys[i] << " = " << values[i] << '\n';
        }
    }

private:
    std::map<std::string, std::string> settings_;
};

int main() {
    Config config;
    config.set("host", "localhost");
    config.set("port", "8080");
    config.set("debug", "true");

    // Safe lookup with fallback
    std::string host = config.get("host")
        || []() { return std::string("127.0.0.1"); };

    std::string timeout = config.get("timeout")
        || []() { return std::string("30"); };

    std::cout << "Host: " << host << '\n';      // localhost
    std::cout << "Timeout: " << timeout << '\n'; // 30 (default)

    // Find which setting has value "true"
    if (auto key = config.find_key_for_value("true")) {
        std::cout << "Setting '" << *key << "' is enabled\n";  // debug
    }

    // List all settings
    std::cout << "\nAll settings:\n";
    config.print_all();
}
```

---

## Performance Considerations

| Function | Time Complexity |
|----------|-----------------|
| `map_lookup_by_key` | O(log n) for `std::map`, O(1) average for `std::unordered_map` |
| `map_lookup_by_value` | O(n) - linear search required |
| `map_keys` | O(n) |
| `map_values` | O(n) |

For frequent value-based lookups, consider maintaining a reverse map or using a bidirectional map structure.

---

## See Also

- [Algorithm Functions](algorithm.md) - General container algorithms
- [OptionalExt](optional.md) - Chaining optional operations
