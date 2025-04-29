#include <gtest/gtest.h>

#include <lbnl/algorithm.hxx>

TEST(MergeTest, EmptyContainers)
{
    std::vector<int> c1 = {};
    std::vector<int> c2 = {};
    auto result = lbnl::merge(c1, c2);
    EXPECT_TRUE(result.empty());
}

TEST(MergeTest, OneEmptyContainer)
{
    std::vector<int> c1 = {};
    std::vector<int> c2 = {1, 2, 3, 4, 5};
    auto result = lbnl::merge(c1, c2);
    EXPECT_EQ(result.size(), 5);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 3);
    EXPECT_EQ(result[3], 4);
    EXPECT_EQ(result[4], 5);
}

TEST(MergeTest, TwoNonEmptyContainers)
{
    std::vector<int> c1 = {1, 3, 5};
    std::vector<int> c2 = {2, 4, 6};
    auto result = lbnl::merge(c1, c2);
    EXPECT_EQ(result.size(), 6);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 3);
    EXPECT_EQ(result[3], 4);
    EXPECT_EQ(result[4], 5);
    EXPECT_EQ(result[5], 6);
}

TEST(MergeTest, ContainersWithDuplicates)
{
    std::vector<int> c1 = {1, 2, 3};
    std::vector<int> c2 = {2, 3, 4};
    auto result = lbnl::merge(c1, c2);
    EXPECT_EQ(result.size(), 6);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 2);
    EXPECT_EQ(result[3], 3);
    EXPECT_EQ(result[4], 3);
    EXPECT_EQ(result[5], 4);
}

TEST(MergeTest, ContainersWithNegativeNumbers)
{
    std::vector<int> c1 = {-1, 1, 3};
    std::vector<int> c2 = {-2, 0, 2};
    auto result = lbnl::merge(c1, c2);
    EXPECT_EQ(result.size(), 6);
    EXPECT_EQ(result[0], -2);
    EXPECT_EQ(result[1], -1);
    EXPECT_EQ(result[2], 0);
    EXPECT_EQ(result[3], 1);
    EXPECT_EQ(result[4], 2);
    EXPECT_EQ(result[5], 3);
}

TEST(MergeTest, ContainersWithFloatingPointNumbers)
{
    std::vector<double> c1 = {1.1, 2.2, 3.3};
    std::vector<double> c2 = {1.2, 2.1, 3.4};
    auto result = lbnl::merge(c1, c2);
    EXPECT_EQ(result.size(), 6);
    EXPECT_DOUBLE_EQ(result[0], 1.1);
    EXPECT_DOUBLE_EQ(result[1], 1.2);
    EXPECT_DOUBLE_EQ(result[2], 2.1);
    EXPECT_DOUBLE_EQ(result[3], 2.2);
    EXPECT_DOUBLE_EQ(result[4], 3.3);
    EXPECT_DOUBLE_EQ(result[5], 3.4);
}

TEST(MergeTest, LargeContainers)
{
    std::vector<int> c1(1000);
    std::vector<int> c2(1000);
    for(int i = 0; i < 1000; ++i)
    {
        c1[i] = i;
        c2[i] = i + 1000;
    }
    auto result = lbnl::merge(c1, c2);
    EXPECT_EQ(result.size(), 2000);
    for(int i = 0; i < 2000; ++i)
    {
        EXPECT_EQ(result[i], i);
    }
}