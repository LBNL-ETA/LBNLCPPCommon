# LazyEvaluator - Memoization and Caching

The `memoize.hxx` header provides `LazyEvaluator<Key, Value>`, a thread-safe memoization utility that caches computed values. It's useful for expensive computations that may be requested multiple times with the same inputs.

## Header

```cpp
#include <lbnl/memoize.hxx>
```

## Overview

| Component | Description |
|-----------|-------------|
| `LazyEvaluator<Key, Value>` | Thread-safe caching evaluator |
| `operator()` | Compute or retrieve cached value |
| `get` | Compute or retrieve cached value (alternative) |

---

## LazyEvaluator Class

`LazyEvaluator<Key, Value>` wraps a generator function and caches its results. When a value is requested for a key:
- If the key exists in the cache, the cached value is returned immediately
- If the key doesn't exist, the generator function is called, the result is cached, and then returned

### Template Parameters

- `Key` - The type used to look up values (must be hashable for `std::unordered_map`)
- `Value` - The type of computed/cached values

### Thread Safety

`LazyEvaluator` uses an internal mutex to ensure thread-safe access to the cache. Multiple threads can safely call `get()` or `operator()` concurrently.

---

## Constructor

```cpp
explicit LazyEvaluator(Generator generator);
```

### Parameters

- `generator` - A callable with signature `Value(const Key&)` that computes values

### Generator Type

```cpp
using Generator = std::function<Value(const Key &)>;
```

### Example

```cpp
#include <lbnl/memoize.hxx>
#include <string>

int main() {
    // Create an evaluator that computes string lengths
    lbnl::LazyEvaluator<std::string, size_t> lengthCache(
        [](const std::string& s) {
            return s.length();
        }
    );
}
```

---

## operator()

Computes or retrieves a cached value for the given key.

```cpp
Value operator()(const Key & key);
```

### Parameters

- `key` - The key to look up or compute a value for

### Returns

The computed or cached value.

### Example

```cpp
#include <lbnl/memoize.hxx>

int main() {
    lbnl::LazyEvaluator<int, long long> fibonacci(
        [](int n) -> long long {
            if (n <= 1) return n;
            long long a = 0, b = 1;
            for (int i = 2; i <= n; ++i) {
                long long next = a + b;
                a = b;
                b = next;
            }
            return b;
        }
    );

    auto fib10 = fibonacci(10);  // Computed: 55
    auto fib10Again = fibonacci(10);  // Cached: 55 (no recomputation)
}
```

---

## get

Alternative method to compute or retrieve a cached value. Returns a const reference.

```cpp
const Value & get(const Key & key);
```

### Parameters

- `key` - The key to look up or compute a value for

### Returns

A const reference to the computed or cached value.

### Example

```cpp
#include <lbnl/memoize.hxx>
#include <string>

int main() {
    lbnl::LazyEvaluator<int, std::string> numberNames(
        [](int n) {
            switch (n) {
                case 1: return std::string("one");
                case 2: return std::string("two");
                case 3: return std::string("three");
                default: return std::string("unknown");
            }
        }
    );

    const std::string& name = numberNames.get(2);  // "two"
}
```

---

## Complete Example: Expensive Computation Cache

```cpp
#include <lbnl/memoize.hxx>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

// Simulated expensive file reading operation
std::string read_file_content(const std::string& path) {
    std::cout << "Reading file: " << path << '\n';
    std::ifstream file(path);
    if (!file) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    // Create a file content cache
    lbnl::LazyEvaluator<std::string, std::string> fileCache(read_file_content);

    // First access - actually reads the file
    auto content1 = fileCache("config.txt");
    // Output: "Reading file: config.txt"

    // Second access - returns cached content
    auto content2 = fileCache("config.txt");
    // No output - cached!

    // Different file - reads it
    auto content3 = fileCache("data.txt");
    // Output: "Reading file: data.txt"

    // Original file again - still cached
    auto content4 = fileCache("config.txt");
    // No output - still cached!
}
```

---

## Example: Database Query Cache

```cpp
#include <lbnl/memoize.hxx>
#include <string>
#include <optional>

struct User {
    int id;
    std::string name;
    std::string email;
};

// Simulated database lookup
User fetch_user_from_db(int userId) {
    // In reality, this would query a database
    std::cout << "Fetching user " << userId << " from database\n";
    return User{userId, "User" + std::to_string(userId), "user@example.com"};
}

int main() {
    // Cache database queries
    lbnl::LazyEvaluator<int, User> userCache(fetch_user_from_db);

    // First request for user 42
    User user1 = userCache(42);
    // Output: "Fetching user 42 from database"

    // Subsequent requests are cached
    User user2 = userCache(42);  // No database query
    User user3 = userCache(42);  // No database query

    // Different user triggers new query
    User user4 = userCache(100);
    // Output: "Fetching user 100 from database"
}
```

---

## Example: Thread-Safe Configuration

```cpp
#include <lbnl/memoize.hxx>
#include <thread>
#include <vector>
#include <iostream>

struct Config {
    std::string database_url;
    int timeout;
    bool debug_mode;
};

Config load_config(const std::string& environment) {
    std::cout << "Loading config for: " << environment << '\n';
    // Simulate expensive config loading
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (environment == "production") {
        return {"prod-db.example.com", 30, false};
    } else {
        return {"localhost", 5, true};
    }
}

int main() {
    lbnl::LazyEvaluator<std::string, Config> configCache(load_config);

    // Multiple threads accessing the same config
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&configCache]() {
            // Thread-safe access
            const Config& config = configCache.get("production");
            std::cout << "Thread got timeout: " << config.timeout << '\n';
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Config was only loaded once despite 10 threads accessing it
}
```

---

## Example: Recursive Computation with Memoization

For recursive algorithms, you can use `LazyEvaluator` to avoid redundant computations:

```cpp
#include <lbnl/memoize.hxx>
#include <functional>

int main() {
    // Self-referencing requires a bit more setup
    std::function<long long(int)> fib;

    lbnl::LazyEvaluator<int, long long> fibCache(
        [&fib](int n) -> long long {
            if (n <= 1) return n;
            return fib(n - 1) + fib(n - 2);
        }
    );

    // Connect the function to the cache
    fib = [&fibCache](int n) { return fibCache(n); };

    // Now fibonacci is memoized
    std::cout << "fib(40) = " << fib(40) << '\n';  // Fast due to memoization
}
```

---

## Key Requirements

### Key Type Requirements

The `Key` type must be:
- Hashable (have a specialization of `std::hash<Key>`)
- Equality comparable (have `operator==`)

Standard types like `int`, `std::string`, and pointers work out of the box. For custom types, you need to provide hash and equality:

```cpp
struct Point {
    int x, y;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// Custom hash
namespace std {
    template<>
    struct hash<Point> {
        size_t operator()(const Point& p) const {
            return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
        }
    };
}

// Now Point can be used as a key
lbnl::LazyEvaluator<Point, double> distanceCache(
    [](const Point& p) {
        return std::sqrt(p.x * p.x + p.y * p.y);
    }
);
```

---

## Performance Characteristics

| Operation | Complexity |
|-----------|------------|
| Cache hit | O(1) average |
| Cache miss | O(1) + generator cost |
| Memory | O(n) where n = unique keys |

### Trade-offs

- **Pros**: Avoids redundant expensive computations
- **Cons**: Memory grows with unique keys; cache is never cleared

For applications needing cache eviction policies (LRU, TTL, etc.), consider using a more sophisticated caching library.

---

## See Also

- [Algorithm Functions](algorithm.md) - Container algorithms
- [OptionalExt](optional.md) - For computations that may fail
