#include <gtest/gtest.h>

#include <lbnl/optional_utils.hxx>

TEST(AverageOptionalTest, AllValuesPresent)
{
    std::vector<std::optional<double>> input = {1.0, 2.0, 3.0, 4.0};
    auto result = lbnl::averageOptional(input);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), 2.5);
}

TEST(AverageOptionalTest, SomeValuesMissing)
{
    std::vector<std::optional<double>> input = {std::nullopt, 2.0, std::nullopt, 6.0};
    auto result = lbnl::averageOptional(input);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), 4.0);  // (2 + 6) / 2
}

TEST(AverageOptionalTest, AllValuesMissing)
{
    std::vector<std::optional<double>> input = {std::nullopt, std::nullopt};
    auto result = lbnl::averageOptional(input);
    EXPECT_FALSE(result.has_value());
}

TEST(AverageOptionalTest, EmptyInput)
{
    std::vector<std::optional<double>> input;
    auto result = lbnl::averageOptional(input);
    EXPECT_FALSE(result.has_value());
}

TEST(AverageOptionalTest, SingleValuePresent)
{
    std::vector<std::optional<double>> input = {std::nullopt, 3.14, std::nullopt};
    auto result = lbnl::averageOptional(input);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), 3.14);
}