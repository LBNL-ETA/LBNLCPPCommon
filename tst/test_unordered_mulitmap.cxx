#include <gtest/gtest.h>
#include <unordered_map>
#include <optional>

#include "lbnl/map_utils.hxx"

TEST(MapUtilsTest, UnorderedMultimapLookupKeyFound)
{
    std::unordered_multimap<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}, {2, "second_two"}};
    auto result = lbnl::map_lookup_by_key(test_map, 2);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value() == "two" || result.value() == "second_two");
}

TEST(MapUtilsTest, UnorderedMultimapLookupKeyNotFound)
{
    std::unordered_multimap<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto result = lbnl::map_lookup_by_key(test_map, 4);
    ASSERT_FALSE(result.has_value());
}

TEST(MapUtilsTest, UnorderedMultimapLookupValueFound)
{
    std::unordered_multimap<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}, {2, "second_two"}};
    auto result = lbnl::map_lookup_by_value(test_map, "three");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);
}

TEST(MapUtilsTest, UnorderedMultimapLookupValueNotFound)
{
    std::unordered_multimap<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto result = lbnl::map_lookup_by_value(test_map, "four");
    ASSERT_FALSE(result.has_value());
}