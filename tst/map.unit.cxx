#include <gtest/gtest.h>
#include <map>
#include <optional>

#include "lbnl/map_utils.hxx"

TEST(MapUtilsTest, MapLookupKeyFound)
{
    std::map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto result = lbnl::map_lookup_by_key(test_map, 2);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "two");
}

TEST(MapUtilsTest, MapLookupKeyNotFound)
{
    std::map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto result = lbnl::map_lookup_by_key(test_map, 4);
    ASSERT_FALSE(result.has_value());
}

TEST(MapUtilsTest, MapLookupValueFound)
{
    std::map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto result = lbnl::map_lookup_by_value(test_map, "three");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);
}

TEST(MapUtilsTest, MapLookupValueNotFound)
{
    std::map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto result = lbnl::map_lookup_by_value(test_map, "four");
    ASSERT_FALSE(result.has_value());
}