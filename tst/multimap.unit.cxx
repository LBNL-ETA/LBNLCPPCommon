#include <gtest/gtest.h>
#include <map>
#include <optional>

#include "lbnl/map_utils.hxx"

TEST(MapUtilsTest, MultimapLookupKeyFound)
{
    std::multimap<int, std::string> test_map = {
      {1, "one"}, {2, "two"}, {3, "three"}, {2, "second_two"}};
    const auto result = lbnl::map_lookup_by_key(test_map, 2);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "two");
}

TEST(MapUtilsTest, MultimapLookupKeyNotFound)
{
    std::multimap<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    const auto result = lbnl::map_lookup_by_key(test_map, 4);
    ASSERT_FALSE(result.has_value());
}

TEST(MapUtilsTest, MultimapLookupValueFound)
{
    std::multimap<int, std::string> test_map = {
      {1, "one"}, {2, "two"}, {3, "three"}, {2, "second_two"}};
    const auto result = lbnl::map_lookup_by_value(test_map, "three");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);
}

TEST(MapUtilsTest, MultimapLookupValueNotFound)
{
    std::multimap<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    const auto result = lbnl::map_lookup_by_value(test_map, "four");
    ASSERT_FALSE(result.has_value());
}

TEST(MapUtilsTest, MapKeysEmpty)
{
    std::map<int, int> m;
    const auto keys = lbnl::map_keys(m);
    EXPECT_TRUE(keys.empty());
}

TEST(MapUtilsTest, MapKeysNonEmpty)
{
    std::map<int, int> m{{1, 10}, {2, 20}};
    const auto keys = lbnl::map_keys(m);
    ASSERT_EQ(keys.size(), 2u);
    EXPECT_EQ(keys[0], 1);
    EXPECT_EQ(keys[1], 2);
}
