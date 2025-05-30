#include <gtest/gtest.h>
#include <vector>
#include <string>

#include <lbnl/algorithm.hxx>

// Test case when an element is found
TEST(FindElementTest, FindsElementInVector)
{
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    auto result =
      lbnl::find_element(numbers, [](int x) { return x % 2 == 0; });   // Find first even number

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);
}

// Test case when an element is not found
TEST(FindElementTest, ReturnsNulloptIfNotFound)
{
    std::vector numbers = {1, 3, 5, 7, 9};   // No even numbers
    auto result = lbnl::find_element(numbers, [](int x) { return x % 2 == 0; });

    EXPECT_FALSE(result.has_value());
}

// Test case with an empty container
TEST(FindElementTest, EmptyContainerReturnsNullopt)
{
    std::vector<int> numbers;
    auto result = lbnl::find_element(numbers, [](int x) { return x == 5; });

    EXPECT_FALSE(result.has_value());
}

// Test case with a vector of strings
TEST(FindElementTest, FindsElementInStringVector)
{
    std::vector<std::string> words = {"apple", "banana", "cherry", "date"};
    auto result =
      lbnl::find_element(words, [](const std::string & s) { return s.starts_with("ch"); });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "cherry");
}

// Test case with a vector of doubles
TEST(FindElementTest, FindsElementInDoubleVector)
{
    std::vector values = {1.1, 2.2, 3.3, 4.4};
    auto result = lbnl::find_element(values, [](double x) { return x > 3.0; });

    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), 3.3);
}

// Test case with custom struct
struct Item
{
    int id;
    std::string name;
};

TEST(FindElementTest, FindsElementInCustomStructVector)
{
    std::vector<Item> items = {{1, "First"}, {2, "Second"}, {3, "Third"}};
    auto result = lbnl::find_element(items, [](const Item & item) { return item.id == 2; });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().name, "Second");
}
