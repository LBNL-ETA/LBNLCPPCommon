#include <gtest/gtest.h>
#include <unordered_map>
#include <optional>

#include "lbnl/map_utils.hxx"

TEST(MapUtilsTest, UnorderedMapLookupKeyFound)
{
    std::unordered_map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto result = lbnl::mapLookupKey(test_map, 2);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "two");
}

TEST(MapUtilsTest, UnorderedMapLookupKeyNotFound)
{
    std::unordered_map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto result = lbnl::mapLookupKey(test_map, 4);
    ASSERT_FALSE(result.has_value());
}

TEST(MapUtilsTest, UnorderedMapLookupValueFound)
{
    std::unordered_map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto result = lbnl::mapLookupValue(test_map, "three");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);
}

TEST(MapUtilsTest, UnorderedMapLookupValueNotFound)
{
    std::unordered_map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto result = lbnl::mapLookupValue(test_map, "four");
    ASSERT_FALSE(result.has_value());
}