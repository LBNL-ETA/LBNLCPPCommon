#include <gtest/gtest.h>
#include "lbnl/algorithm.hxx"

TEST(SplitViewTest, BasicSplitting) {
    std::string input = "apple,banana,orange";
    auto result = lbnl::split_view(input, ',');

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "apple");
    EXPECT_EQ(result[1], "banana");
    EXPECT_EQ(result[2], "orange");
}

TEST(SplitViewTest, SingleElement) {
    std::string input = "singleword";
    auto result = lbnl::split_view(input, ',');

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "singleword");
}

TEST(SplitViewTest, EmptyString) {
    std::string input = "";
    auto result = lbnl::split_view(input, ',');

    ASSERT_TRUE(result.empty());
}

TEST(SplitViewTest, ConsecutiveDelimiters) {
    std::string input = "apple,,banana,,orange";
    auto result = lbnl::split_view(input, ',');

    ASSERT_EQ(result.size(), 5);
    EXPECT_EQ(result[0], "apple");
    EXPECT_EQ(result[1], "");
    EXPECT_EQ(result[2], "banana");
    EXPECT_EQ(result[3], "");
    EXPECT_EQ(result[4], "orange");
}

TEST(SplitViewTest, LeadingAndTrailingDelimiters) {
    std::string input = ",apple,banana,orange,";
    auto result = lbnl::split_view(input, ',');

    ASSERT_EQ(result.size(), 5);
    EXPECT_EQ(result[0], "");
    EXPECT_EQ(result[1], "apple");
    EXPECT_EQ(result[2], "banana");
    EXPECT_EQ(result[3], "orange");
    EXPECT_EQ(result[4], "");
}

TEST(SplitViewTest, DifferentDelimiter) {
    std::string input = "one|two|three|four";
    auto result = lbnl::split_view(input, '|');

    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(result[0], "one");
    EXPECT_EQ(result[1], "two");
    EXPECT_EQ(result[2], "three");
    EXPECT_EQ(result[3], "four");
}
