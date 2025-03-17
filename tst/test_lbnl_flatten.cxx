#include <gtest/gtest.h>
#include <lbnl/algorithm.hxx>

TEST(FlattenTest, BasicFlatten)
{
    std::vector<std::vector<int>> nested = {{1, 2}, {3, 4}, {5}};
    auto result = lbnl::flatten(nested);

    std::vector<int> expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(result, expected);
}

TEST(FlattenTest, EmptyNestedVector)
{
    std::vector<std::vector<int>> nested = {};
    auto result = lbnl::flatten(nested);

    EXPECT_TRUE(result.empty());
}

TEST(FlattenTest, VectorWithEmptySubVectors)
{
    std::vector<std::vector<int>> nested = {{}, {1, 2, 3}, {}, {4, 5}};
    auto result = lbnl::flatten(nested);

    std::vector<int> expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(result, expected);
}

TEST(FlattenTest, NestedStrings)
{
    std::vector<std::vector<std::string>> nested = {
      {"apple", "banana"}, {"cherry"}, {"date", "fig"}};
    auto result = lbnl::flatten(nested);

    std::vector<std::string> expected = {"apple", "banana", "cherry", "date", "fig"};
    EXPECT_EQ(result, expected);
}

TEST(FlattenTest, NestedDoubles)
{
    std::vector<std::vector<double>> nested = {{1.1, 2.2}, {3.3}, {4.4, 5.5}};
    auto result = lbnl::flatten(nested);

    std::vector<double> expected = {1.1, 2.2, 3.3, 4.4, 5.5};
    EXPECT_EQ(result, expected);
}

TEST(FlattenTest, SingleElementVectors)
{
    std::vector<std::vector<int>> nested = {{1}, {2}, {3}, {4}};
    auto result = lbnl::flatten(nested);

    std::vector<int> expected = {1, 2, 3, 4};
    EXPECT_EQ(result, expected);
}

TEST(FlattenTest, AlreadyFlattened)
{
    std::vector<std::vector<int>> nested = {{1, 2, 3, 4, 5}};
    auto result = lbnl::flatten(nested);

    std::vector<int> expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(result, expected);
}
