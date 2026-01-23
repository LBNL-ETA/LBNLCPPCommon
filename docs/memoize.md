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
| `get` | Compute or retrieve cached value (returns reference) |

---

## LazyEvaluator Class

`LazyEvaluator<Key, Value>` wraps a generator function and caches its results. When a value is requested:
- If the key exists in cache, the cached value is returned immediately
- If the key doesn't exist, the generator is called, result is cached, then returned

### Thread Safety

Uses an internal mutex to ensure thread-safe access to the cache.

---

## Constructor

```cpp
explicit LazyEvaluator(Generator generator);
```

Where `Generator` is `std::function<Value(const Key&)>`.

---

## operator() / get

```cpp
Value operator()(const Key & key);
const Value & get(const Key & key);
```

### Example

```cpp
#include <lbnl/memoize.hxx>
#include <string>

// Expensive computation
std::string fetch_data(int id) {
    std::cout << "Fetching " << id << "...\n";
    return "Data for " + std::to_string(id);
}

int main() {
    lbnl::LazyEvaluator<int, std::string> cache(fetch_data);

    auto data1 = cache(42);  // Fetches and caches
    auto data2 = cache(42);  // Returns cached (no fetch)
    auto data3 = cache(100); // Fetches new key
}
```

---

## Complete Example: File Content Cache

```cpp
#include <lbnl/memoize.hxx>
#include <fstream>
#include <sstream>

std::string read_file(const std::string& path) {
    std::cout << "Reading: " << path << '\n';
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    lbnl::LazyEvaluator<std::string, std::string> fileCache(read_file);

    // First access reads file
    auto content1 = fileCache("config.txt");

    // Second access returns cached content
    auto content2 = fileCache("config.txt");

    // Different file triggers new read
    auto content3 = fileCache("data.txt");
}
```

---

## Key Requirements

The `Key` type must be:
- Hashable (`std::hash<Key>` specialization)
- Equality comparable (`operator==`)

Standard types like `int`, `std::string` work out of the box.

---

## See Also

- [Algorithm Functions](algorithm.md) - Container algorithms
- [OptionalExt](optional.md) - For computations that may fail
