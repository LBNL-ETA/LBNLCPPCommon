# Algorithm Functions

The `algorithm.hxx` header provides generic container and range algorithms that work with standard C++ containers using modern C++20 features like concepts and ranges.

## Header

```cpp
#include <lbnl/algorithm.hxx>
```

## Functions Overview

| Function | Description |
|----------|-------------|
| `find_element` | Find first element matching a predicate |
| `contains` | Check if container contains a value |
| `sorted_unique` | Remove duplicate elements (sorts first) |
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

---

## find_element

Finds the first element in a container that satisfies a given predicate.

```cpp
template<typename Container, std::predicate<const typename Container::value_type &> Predicate>
[[nodiscard]] constexpr std::optional<std::decay_t<typename Container::value_type>>
find_element(const Container & elements, Predicate predicate);
```

### Parameters

- `elements` - The container to search through
- `predicate` - A callable that returns `true` for the desired element

### Returns

`std::optional<T>` containing the first matching element, or `std::nullopt` if not found.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5};

    // Find first element greater than 3
    auto found = lbnl::find_element(numbers, [](int x) { return x > 3; });

    if (found) {
        std::cout << "Found: " << *found << '\n';  // Output: Found: 4
    }

    // Find element that doesn't exist
    auto notFound = lbnl::find_element(numbers, [](int x) { return x > 10; });

    if (!notFound) {
        std::cout << "Not found\n";
    }
}
```

---

## contains

Checks if a container contains a specific value.

```cpp
template<typename Container, typename T>
[[nodiscard]] constexpr bool contains(const Container & elements, const T & value);
```

### Parameters

- `elements` - The container to search through
- `value` - The value to search for

### Returns

`true` if the value is found, `false` otherwise.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> fruits = {"apple", "banana", "cherry"};

    bool hasApple = lbnl::contains(fruits, std::string("apple"));   // true
    bool hasOrange = lbnl::contains(fruits, std::string("orange")); // false
}
```

---

## sorted_unique

Removes duplicate elements from a range, returning a new sorted container.

```cpp
template<std::ranges::range R>
[[nodiscard]] constexpr auto sorted_unique(const R & range);
```

### Parameters

- `range` - The range of elements to process

### Returns

A new `std::vector` with duplicates removed (sorted).

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>

int main() {
    std::vector<int> numbers = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};

    auto uniqueNumbers = lbnl::sorted_unique(numbers);
    // Result: {1, 2, 3, 4, 5, 6, 9}
}
```

---

## zip

Combines two ranges into a single container of pairs.

```cpp
template<std::ranges::input_range R1, std::ranges::input_range R2>
[[nodiscard]] auto zip(R1 && r1, R2 && r2);
```

### Parameters

- `r1` - The first range
- `r2` - The second range

### Returns

A `std::vector<std::pair<T1, T2>>` where each pair contains one element from each range. The result size equals the smaller of the two input ranges.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
    std::vector<int> ages = {25, 30, 35};

    auto zipped = lbnl::zip(names, ages);
    // Result: {{"Alice", 25}, {"Bob", 30}, {"Charlie", 35}}

    for (const auto& [name, age] : zipped) {
        std::cout << name << " is " << age << " years old\n";
    }
}
```

---

## filter

Filters elements in a range based on a predicate.

```cpp
template<std::ranges::range R, typename Predicate>
[[nodiscard]] constexpr auto filter(const R & range, Predicate predicate);
```

### Parameters

- `range` - The range of elements to filter
- `predicate` - A callable that returns `true` for elements to include

### Returns

A `std::vector` containing only elements that satisfy the predicate.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>

int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Get only even numbers
    auto evens = lbnl::filter(numbers, [](int x) { return x % 2 == 0; });
    // Result: {2, 4, 6, 8, 10}
}
```

---

## transform_if

Applies a transformation to elements that satisfy a predicate, copying others unchanged.

```cpp
template<std::ranges::range R, typename Predicate, typename Func>
[[nodiscard]] constexpr auto transform_if(const R & range, Predicate pred, Func func);
```

### Parameters

- `range` - The input range to process
- `pred` - Predicate to determine which elements to transform
- `func` - Transformation function applied to matching elements

### Returns

A `std::vector` where matching elements are transformed and others are copied as-is.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>

int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5};

    // Double only even numbers
    auto result = lbnl::transform_if(
        numbers,
        [](int x) { return x % 2 == 0; },  // predicate
        [](int x) { return x * 2; }         // transform
    );
    // Result: {1, 4, 3, 8, 5}
}
```

---

## transform_filter

Applies a transformation to elements that satisfy a predicate, excluding non-matching elements.

```cpp
template<std::ranges::range R, typename Predicate, typename Func>
[[nodiscard]] auto transform_filter(const R & range, Predicate pred, Func func);
```

### Parameters

- `range` - The input range to process
- `pred` - Predicate used to select elements
- `func` - Function used to transform selected elements

### Returns

A `std::vector` of transformed elements that passed the predicate.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>
#include <string>

struct Person {
    std::string name;
    int age;
};

int main() {
    std::vector<Person> people = {
        {"Alice", 25}, {"Bob", 17}, {"Charlie", 30}, {"Diana", 15}
    };

    // Get names of adults only
    auto adultNames = lbnl::transform_filter(
        people,
        [](const Person& p) { return p.age >= 18; },
        [](const Person& p) { return p.name; }
    );
    // Result: {"Alice", "Charlie"}
}
```

---

## merge

Merges two sorted ranges into a single sorted range.

```cpp
template<std::ranges::range R1, std::ranges::range R2>
[[nodiscard]] constexpr auto merge(const R1 & range1, const R2 & range2);
```

### Parameters

- `range1` - First sorted range
- `range2` - Second sorted range

### Returns

A `std::vector` containing all elements from both ranges in sorted order.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>

int main() {
    std::vector<int> a = {1, 3, 5, 7};
    std::vector<int> b = {2, 4, 6, 8};

    auto merged = lbnl::merge(a, b);
    // Result: {1, 2, 3, 4, 5, 6, 7, 8}
}
```

---

## split

Splits a string into a vector of substrings based on a delimiter.

```cpp
template<typename Str, typename CharT = typename std::decay_t<Str>::value_type>
[[nodiscard]] constexpr std::vector<std::basic_string<CharT>> split(Str && str, CharT delimiter);
```

### Parameters

- `str` - The string to split
- `delimiter` - The character used to split the string

### Returns

A `std::vector` of substrings.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <string>

int main() {
    std::string csv = "apple,banana,cherry,date";

    auto fruits = lbnl::split(csv, ',');
    // Result: {"apple", "banana", "cherry", "date"}

    // Works with empty segments
    std::string data = "a,,b,c";
    auto parts = lbnl::split(data, ',');
    // Result: {"a", "", "b", "c"}
}
```

---

## partition

Partitions a range into two groups based on a predicate.

```cpp
template<std::ranges::range R, typename Predicate>
[[nodiscard]] constexpr auto partition(const R & range, Predicate predicate);
```

### Parameters

- `range` - The range of elements to partition
- `predicate` - The condition to partition elements

### Returns

A `std::pair` of vectors where:
- `first` contains elements that satisfy the predicate
- `second` contains elements that don't

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>

int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto [evens, odds] = lbnl::partition(numbers, [](int x) { return x % 2 == 0; });
    // evens: {2, 4, 6, 8, 10}
    // odds:  {1, 3, 5, 7, 9}
}
```

---

## flatten

Flattens a nested vector into a single vector.

```cpp
template<typename T>
[[nodiscard]] constexpr std::vector<T> flatten(const std::vector<std::vector<T>> & nested);
```

### Parameters

- `nested` - The nested vector to flatten

### Returns

A single `std::vector<T>` containing all elements from all inner vectors.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>

int main() {
    std::vector<std::vector<int>> nested = {
        {1, 2, 3},
        {4, 5},
        {6, 7, 8, 9}
    };

    auto flat = lbnl::flatten(nested);
    // Result: {1, 2, 3, 4, 5, 6, 7, 8, 9}
}
```

---

## to_vector

Converts any range into a vector.

```cpp
template<std::ranges::input_range R>
[[nodiscard]] constexpr auto to_vector(R && r);
```

### Parameters

- `r` - The range to convert

### Returns

A `std::vector<T>` containing all elements from the range.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <array>
#include <set>

int main() {
    std::array<int, 5> arr = {1, 2, 3, 4, 5};
    auto vecFromArray = lbnl::to_vector(arr);

    std::set<std::string> set = {"apple", "banana", "cherry"};
    auto vecFromSet = lbnl::to_vector(set);
}
```

---

## transform_to_vector

Transforms a range into a vector by applying a function to each element.

```cpp
template<std::ranges::input_range R, typename Func>
[[nodiscard]] constexpr auto transform_to_vector(R && range, Func && func);
```

### Parameters

- `range` - The range of elements to transform
- `func` - The transformation function to apply

### Returns

A `std::vector` containing the transformed elements.

### Example

```cpp
#include <lbnl/algorithm.hxx>
#include <vector>
#include <string>

int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5};

    // Convert to strings
    auto strings = lbnl::transform_to_vector(
        numbers,
        [](int x) { return std::to_string(x); }
    );
    // Result: {"1", "2", "3", "4", "5"}

    // Square each number
    auto squares = lbnl::transform_to_vector(
        numbers,
        [](int x) { return x * x; }
    );
    // Result: {1, 4, 9, 16, 25}
}
```

---

## See Also

- [OptionalExt](optional.md) - Extended optional with monadic operations
- [Map Utilities](map_utils.md) - Utilities for associative containers
