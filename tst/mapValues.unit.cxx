#include <gtest/gtest.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "lbnl/map_utils.hxx"

TEST(MapUtilsTest, MapValues)
{
    std::map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto values = lbnl::map_values(test_map);
    std::vector<std::string> expected_values = {"one", "two", "three"};
    EXPECT_EQ(values, expected_values);
}

TEST(MapUtilsTest, UnorderedMapValues)
{
    std::unordered_map<int, std::string> test_map = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto values = lbnl::map_values(test_map);
    std::vector<std::string> expected_values = {"one", "three", "two"};
    std::ranges::sort(values);
    EXPECT_EQ(values, expected_values);
}

TEST(MapUtilsTest, MultimapValues)
{
    std::multimap<int, std::string> test_map = {
      {1, "one"}, {2, "two"}, {3, "three"}, {2, "second_two"}};
    auto values = lbnl::map_values(test_map);
    std::vector<std::string> expected_values = {"one", "two", "second_two", "three"};
    EXPECT_EQ(values, expected_values);
}

TEST(MapUtilsTest, UnorderedMultimapValues)
{
    std::unordered_multimap<int, std::string> test_map = {
      {1, "one"}, {2, "two"}, {3, "three"}, {2, "second_two"}};
    auto values = lbnl::map_values(test_map);
    std::vector<std::string> expected_values = {"one", "second_two", "three", "two"};
    std::ranges::sort(values);
    EXPECT_EQ(values, expected_values);
}