#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <ranges>
#include <lbnl/algorithm.hxx>   // Adjust to match your actual include path

TEST(ToVectorTest, ConvertsRangeToVectorCorrectly)
{
    std::vector<int> input = {1, 2, 3, 4};
    auto r = input | std::views::filter([](int x) { return x % 2 == 0; });

    auto result = lbnl::to_vector(r);

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 2);
    EXPECT_EQ(result[1], 4);
}

TEST(ToVectorTest, WorksWithStringViewRange)
{
    std::string_view str = "abc";
    auto result = lbnl::to_vector(str);

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 'a');
    EXPECT_EQ(result[1], 'b');
    EXPECT_EQ(result[2], 'c');
}

TEST(TransformToVectorTest, TransformsAndCollects)
{
    std::vector<std::string> input = {"a", "ab", "abc"};

    auto lengths = lbnl::transform_to_vector(input, [](const std::string & s) { return s.size(); });

    ASSERT_EQ(lengths.size(), 3);
    EXPECT_EQ(lengths[0], 1);
    EXPECT_EQ(lengths[1], 2);
    EXPECT_EQ(lengths[2], 3);
}

TEST(TransformToVectorTest, WorksWithInlineRange)
{
    auto result =
      lbnl::transform_to_vector(std::vector<int>{1, 2, 3}, [](int x) { return std::to_string(x); });

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "1");
    EXPECT_EQ(result[1], "2");
    EXPECT_EQ(result[2], "3");
}
