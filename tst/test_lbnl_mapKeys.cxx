#include <gtest/gtest.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "lbnl/map_utils.hxx"

TEST(MapUtilsTest, MapKeys)
{
    std::map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto keys = lbnl::mapKeys(test_map);
    std::vector<int> expected_keys = {1, 2, 3};
    EXPECT_EQ(keys, expected_keys);
}

TEST(MapUtilsTest, UnorderedMapKeys)
{
    std::unordered_map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto keys = lbnl::mapKeys(test_map);
    std::vector<int> expected_keys = {1, 2, 3};
    std::ranges::sort(keys);
    EXPECT_EQ(keys, expected_keys);
}

TEST(MapUtilsTest, MultimapKeys)
{
    std::multimap<int, std::string> test_map = {
      {1, "one"}, {2, "two"}, {3, "three"}, {2, "second_two"}};
    auto keys = lbnl::mapKeys(test_map);
    std::vector<int> expected_keys = {1, 2, 2, 3};
    EXPECT_EQ(keys, expected_keys);
}

TEST(MapUtilsTest, UnorderedMultimapKeys)
{
    std::unordered_multimap<int, std::string> test_map = {
      {1, "one"}, {2, "two"}, {3, "three"}, {2, "second_two"}};
    auto keys = lbnl::mapKeys(test_map);
    std::vector<int> expected_keys = {1, 2, 2, 3};
    std::ranges::sort(keys);
    EXPECT_EQ(keys, expected_keys);
}