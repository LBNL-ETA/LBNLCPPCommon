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

## Enum Concept

The library constrains `EnumIndexMapper` to work only with enum types using a C++20 concept:

```cpp
template<class E>
concept Enum = std::is_enum_v<E>;
```

---

## Creating an EnumIndexMapper

### Direct Construction

```cpp
#include <lbnl/enum_index_mapper.hxx>
#include <array>

enum class Color { Red, Green, Blue };

int main() {
    // Define the mapping as an array of (index, enum) pairs
    constexpr std::array<std::pair<int, Color>, 3> mapping = {{
        {1, Color::Red},
        {2, Color::Green},
        {3, Color::Blue}
    }};

    constexpr lbnl::EnumIndexMapper<Color, 3> mapper(mapping);
}
```

### Using make_enum_index_mapper

The helper function deduces template parameters automatically:

```cpp
#include <lbnl/enum_index_mapper.hxx>

enum class Status { Pending, Active, Completed, Cancelled };

int main() {
    // From std::array
    constexpr auto mapper1 = lbnl::make_enum_index_mapper(
        std::array<std::pair<int, Status>, 4>{{
            {0, Status::Pending},
            {1, Status::Active},
            {2, Status::Completed},
            {3, Status::Cancelled}
        }}
    );

    // From brace-initialized array (simpler syntax)
    constexpr auto mapper2 = lbnl::make_enum_index_mapper<Status>({
        {0, Status::Pending},
        {1, Status::Active},
        {2, Status::Completed},
        {3, Status::Cancelled}
    });
}
```

---

## to_enum

Converts an integer index to its corresponding enum value.

```cpp
[[nodiscard]] constexpr std::optional<E> to_enum(int index) const;
```

### Parameters

- `index` - The integer index to convert

### Returns

`std::optional<E>` containing the enum value if the index exists in the mapping, or `std::nullopt` if not found.

### Example

```cpp
#include <lbnl/enum_index_mapper.hxx>

enum class Priority { Low, Medium, High, Critical };

int main() {
    constexpr auto mapper = lbnl::make_enum_index_mapper<Priority>({
        {1, Priority::Low},
        {2, Priority::Medium},
        {3, Priority::High},
        {4, Priority::Critical}
    });

    auto priority = mapper.to_enum(3);
    if (priority) {
        // *priority == Priority::High
    }

    auto unknown = mapper.to_enum(99);
    // unknown == std::nullopt
}
```

---

## to_index

Converts an enum value to its corresponding integer index.

```cpp
[[nodiscard]] constexpr std::optional<int> to_index(E e) const;
```

### Parameters

- `e` - The enum value to convert

### Returns

`std::optional<int>` containing the index if the enum exists in the mapping, or `std::nullopt` if not found.

### Example

```cpp
#include <lbnl/enum_index_mapper.hxx>

enum class Priority { Low, Medium, High, Critical };

int main() {
    constexpr auto mapper = lbnl::make_enum_index_mapper<Priority>({
        {1, Priority::Low},
        {2, Priority::Medium},
        {3, Priority::High},
        {4, Priority::Critical}
    });

    auto index = mapper.to_index(Priority::High);
    if (index) {
        // *index == 3
    }
}
```

---

## to_enum_or

Converts an integer index to enum with a fallback value for unknown indices.

```cpp
[[nodiscard]] constexpr E to_enum_or(int index, E fallback) const;
```

### Parameters

- `index` - The integer index to convert
- `fallback` - The enum value to return if index is not found

### Returns

The corresponding enum value, or `fallback` if the index doesn't exist.

### Example

```cpp
#include <lbnl/enum_index_mapper.hxx>

enum class LogLevel { Debug, Info, Warning, Error, Unknown };

int main() {
    constexpr auto mapper = lbnl::make_enum_index_mapper<LogLevel>({
        {0, LogLevel::Debug},
        {1, LogLevel::Info},
        {2, LogLevel::Warning},
        {3, LogLevel::Error}
    });

    // Known index
    auto level1 = mapper.to_enum_or(2, LogLevel::Unknown);
    // level1 == LogLevel::Warning

    // Unknown index - returns fallback
    auto level2 = mapper.to_enum_or(99, LogLevel::Unknown);
    // level2 == LogLevel::Unknown
}
```

---

## to_index_or

Converts an enum value to index with a fallback value for unknown enums.

```cpp
[[nodiscard]] constexpr int to_index_or(E e, int fallback) const;
```

### Parameters

- `e` - The enum value to convert
- `fallback` - The index to return if enum is not found

### Returns

The corresponding index, or `fallback` if the enum doesn't exist in the mapping.

### Example

```cpp
#include <lbnl/enum_index_mapper.hxx>

enum class FileType { Text, Binary, Image, Video, Other };

int main() {
    constexpr auto mapper = lbnl::make_enum_index_mapper<FileType>({
        {1, FileType::Text},
        {2, FileType::Binary},
        {3, FileType::Image},
        {4, FileType::Video}
    });

    // Known enum
    int index1 = mapper.to_index_or(FileType::Image, -1);
    // index1 == 3

    // Unknown enum (not in mapping)
    int index2 = mapper.to_index_or(FileType::Other, -1);
    // index2 == -1
}
```

---

## data

Returns the underlying mapping array for iteration or testing.

```cpp
[[nodiscard]] constexpr storage_t const & data() const;
```

### Example

```cpp
#include <lbnl/enum_index_mapper.hxx>

enum class Day { Mon, Tue, Wed, Thu, Fri, Sat, Sun };

int main() {
    constexpr auto mapper = lbnl::make_enum_index_mapper<Day>({
        {1, Day::Mon}, {2, Day::Tue}, {3, Day::Wed},
        {4, Day::Thu}, {5, Day::Fri}, {6, Day::Sat}, {7, Day::Sun}
    });

    // Iterate over all mappings
    for (const auto& [index, day] : mapper.data()) {
        std::cout << "Index " << index << " maps to day enum\n";
    }
}
```

---

## Complete Example: Database Entity Mapping

```cpp
#include <lbnl/enum_index_mapper.hxx>
#include <string>
#include <iostream>

// Domain enum
enum class OrderStatus {
    Draft,
    Submitted,
    Processing,
    Shipped,
    Delivered,
    Cancelled
};

// Database uses different integer codes
constexpr auto statusMapper = lbnl::make_enum_index_mapper<OrderStatus>({
    {0,  OrderStatus::Draft},
    {10, OrderStatus::Submitted},
    {20, OrderStatus::Processing},
    {30, OrderStatus::Shipped},
    {40, OrderStatus::Delivered},
    {99, OrderStatus::Cancelled}
});

// Simulated database record
struct OrderRecord {
    int id;
    int status_code;  // Database stores integer
    std::string customer;
};

// Domain object
struct Order {
    int id;
    OrderStatus status;  // Code uses enum
    std::string customer;
};

// Convert from database record to domain object
std::optional<Order> to_domain(const OrderRecord& record) {
    auto status = statusMapper.to_enum(record.status_code);
    if (!status) {
        return std::nullopt;  // Unknown status code
    }
    return Order{record.id, *status, record.customer};
}

// Convert from domain object to database record
OrderRecord to_record(const Order& order) {
    return OrderRecord{
        order.id,
        statusMapper.to_index_or(order.status, 0),  // Default to Draft
        order.customer
    };
}

int main() {
    // Load from "database"
    OrderRecord dbRecord{123, 30, "Alice"};

    if (auto order = to_domain(dbRecord)) {
        std::cout << "Order " << order->id << " status: ";
        switch (order->status) {
            case OrderStatus::Shipped:
                std::cout << "Shipped\n";
                break;
            // ... other cases
        }

        // Modify and save back
        order->status = OrderStatus::Delivered;
        auto newRecord = to_record(*order);
        std::cout << "New status code: " << newRecord.status_code << '\n';  // 40
    }
}
```

---

## Compile-Time Usage

`EnumIndexMapper` is fully `constexpr`, enabling compile-time mapping:

```cpp
#include <lbnl/enum_index_mapper.hxx>

enum class Direction { North, East, South, West };

constexpr auto dirMapper = lbnl::make_enum_index_mapper<Direction>({
    {0, Direction::North},
    {1, Direction::East},
    {2, Direction::South},
    {3, Direction::West}
});

// Compile-time conversion
constexpr auto dir = dirMapper.to_enum_or(2, Direction::North);
static_assert(dir == Direction::South, "Should be South");

constexpr auto idx = dirMapper.to_index_or(Direction::East, -1);
static_assert(idx == 1, "East should be index 1");
```

---

## Performance

- Lookups are O(N) linear scans through the mapping array
- This is efficient for small enums (typical use case)
- The constexpr-friendly design allows compile-time evaluation
- For large enums with frequent lookups at runtime, consider using `std::unordered_map` instead

---

## See Also

- [Map Utilities](map_utils.md) - General map operations
- [OptionalExt](optional.md) - Chaining optional operations
