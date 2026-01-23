# EnumIndexMapper - Bidirectional Enum Mapping

The `enum_index_mapper.hxx` header provides `EnumIndexMapper<E, N>`, a class for bidirectional mapping between database indices (integers) and enum values. This is useful when persisting enum values to databases or external systems that use numeric identifiers.

## Header

```cpp
#include <lbnl/enum_index_mapper.hxx>
```

## Overview

| Component | Description |
|-----------|-------------|
| `EnumIndexMapper<E, N>` | Bidirectional mapper class |
| `to_enum` | Convert index to enum (returns optional) |
| `to_index` | Convert enum to index (returns optional) |
| `to_enum_or` | Convert with fallback enum value |
| `to_index_or` | Convert with fallback index |
| `make_enum_index_mapper` | Helper to create mapper |

---

## Creating an EnumIndexMapper

```cpp
#include <lbnl/enum_index_mapper.hxx>

enum class Status { Pending, Active, Completed, Cancelled };

// Using make_enum_index_mapper helper
constexpr auto mapper = lbnl::make_enum_index_mapper<Status>({
    {0, Status::Pending},
    {1, Status::Active},
    {2, Status::Completed},
    {3, Status::Cancelled}
});
```

---

## to_enum

Converts an integer index to its corresponding enum value.

```cpp
[[nodiscard]] constexpr std::optional<E> to_enum(int index) const;
```

### Example

```cpp
auto status = mapper.to_enum(2);
if (status) {
    // *status == Status::Completed
}

auto unknown = mapper.to_enum(99);
// unknown == std::nullopt
```

---

## to_index

Converts an enum value to its corresponding integer index.

```cpp
[[nodiscard]] constexpr std::optional<int> to_index(E e) const;
```

### Example

```cpp
auto index = mapper.to_index(Status::Active);
if (index) {
    // *index == 1
}
```

---

## to_enum_or / to_index_or

Convert with fallback values for unknown inputs.

```cpp
[[nodiscard]] constexpr E to_enum_or(int index, E fallback) const;
[[nodiscard]] constexpr int to_index_or(E e, int fallback) const;
```

### Example

```cpp
auto status = mapper.to_enum_or(99, Status::Pending);
// status == Status::Pending (fallback)

int index = mapper.to_index_or(Status::Active, -1);
// index == 1
```

---

## Complete Example

```cpp
#include <lbnl/enum_index_mapper.hxx>
#include <iostream>

enum class Priority { Low, Medium, High, Critical };

constexpr auto priorityMapper = lbnl::make_enum_index_mapper<Priority>({
    {1, Priority::Low},
    {2, Priority::Medium},
    {3, Priority::High},
    {4, Priority::Critical}
});

int main() {
    // Database returns priority code 3
    int dbCode = 3;

    auto priority = priorityMapper.to_enum(dbCode);
    if (priority && *priority == Priority::High) {
        std::cout << "High priority item!\n";
    }

    // Convert enum back to database code
    int code = priorityMapper.to_index_or(Priority::Critical, 0);
    std::cout << "Critical priority code: " << code << '\n';  // 4
}
```

---

## See Also

- [Map Utilities](map_utils.md) - General map operations
- [OptionalExt](optional.md) - Chaining optional operations
