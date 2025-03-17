#include <gtest/gtest.h>

#include <lbnl/algorithm.hxx>

TEST(FilterTest, EmptyContainer)
{
    std::vector<int> c = {};
    auto result = lbnl::filter(c, [](int x) { return x > 0; });
    EXPECT_TRUE(result.empty());
}

TEST(FilterTest, AllElementsPass)
{
    std::vector<int> c = {1, 2, 3, 4, 5};
    auto result = lbnl::filter(c, [](int x) { return x > 0; });
    EXPECT_EQ(result.size(), 5);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 3);
    EXPECT_EQ(result[3], 4);
    EXPECT_EQ(result[4], 5);
}

TEST(FilterTest, NoElementsPass)
{
    std::vector<int> c = {1, 2, 3, 4, 5};
    auto result = lbnl::filter(c, [](int x) { return x > 10; });
    EXPECT_TRUE(result.empty());
}

TEST(FilterTest, SomeElementsPass)
{
    std::vector<int> c = {1, 2, 3, 4, 5};
    auto result = lbnl::filter(c, [](int x) { return x > 3; });
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 4);
    EXPECT_EQ(result[1], 5);
}

TEST(FilterTest, NegativeNumbers)
{
    std::vector<int> c = {-1, -2, -3, -4, -5};
    auto result = lbnl::filter(c, [](int x) { return x < 0; });
    EXPECT_EQ(result.size(), 5);
    EXPECT_EQ(result[0], -1);
    EXPECT_EQ(result[1], -2);
    EXPECT_EQ(result[2], -3);
    EXPECT_EQ(result[3], -4);
    EXPECT_EQ(result[4], -5);
}

TEST(FilterTest, FloatingPointNumbers)
{
    std::vector<double> c = {1.1, 2.2, 3.3, 4.4, 5.5};
    auto result = lbnl::filter(c, [](double x) { return x > 3.0; });
    EXPECT_EQ(result.size(), 3);
    EXPECT_DOUBLE_EQ(result[0], 3.3);
    EXPECT_DOUBLE_EQ(result[1], 4.4);
    EXPECT_DOUBLE_EQ(result[2], 5.5);
}

TEST(FilterTest, StringContainer)
{
    std::vector<std::string> c = {"apple", "banana", "cherry", "date", "elderberry"};
    auto result = lbnl::filter(c, [](const std::string & x) { return x.size() > 5; });
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "banana");
    EXPECT_EQ(result[1], "cherry");
    EXPECT_EQ(result[2], "elderberry");
}