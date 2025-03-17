#include <gtest/gtest.h>

#include <lbnl/algorithm.hxx>

TEST(PartitionTest, EmptyContainer)
{
    std::vector<int> c = {};
    auto result = lbnl::partition(c, [](int x) { return x > 0; });
    EXPECT_TRUE(result.first.empty());
    EXPECT_TRUE(result.second.empty());
}

TEST(PartitionTest, AllElementsPass)
{
    std::vector<int> c = {1, 2, 3, 4, 5};
    auto result = lbnl::partition(c, [](int x) { return x > 0; });
    EXPECT_EQ(result.first.size(), 5);
    EXPECT_TRUE(result.second.empty());
}

TEST(PartitionTest, NoElementsPass)
{
    std::vector<int> c = {-1, -2, -3, -4, -5};
    auto result = lbnl::partition(c, [](int x) { return x > 0; });
    EXPECT_TRUE(result.first.empty());
    EXPECT_EQ(result.second.size(), 5);
}

TEST(PartitionTest, SomeElementsPass)
{
    std::vector<int> c = {-1, 1, -2, 2, -3, 3};
    auto result = lbnl::partition(c, [](int x) { return x > 0; });
    EXPECT_EQ(result.first.size(), 3);
    EXPECT_EQ(result.second.size(), 3);
    EXPECT_EQ(result.first[0], 1);
    EXPECT_EQ(result.first[1], 2);
    EXPECT_EQ(result.first[2], 3);
    EXPECT_EQ(result.second[0], -1);
    EXPECT_EQ(result.second[1], -2);
    EXPECT_EQ(result.second[2], -3);
}

TEST(PartitionTest, ContainersWithDuplicates)
{
    std::vector<int> c = {1, 1, 2, 2, -1, -1};
    auto result = lbnl::partition(c, [](int x) { return x > 0; });
    EXPECT_EQ(result.first.size(), 4);
    EXPECT_EQ(result.second.size(), 2);
    EXPECT_EQ(result.first[0], 1);
    EXPECT_EQ(result.first[1], 1);
    EXPECT_EQ(result.first[2], 2);
    EXPECT_EQ(result.first[3], 2);
    EXPECT_EQ(result.second[0], -1);
    EXPECT_EQ(result.second[1], -1);
}

TEST(PartitionTest, ContainersWithNegativeNumbers)
{
    std::vector<int> c = {-1, -2, -3, -4, -5};
    auto result = lbnl::partition(c, [](int x) { return x < -3; });
    EXPECT_EQ(result.first.size(), 2);
    EXPECT_EQ(result.second.size(), 3);
    EXPECT_EQ(result.first[0], -4);
    EXPECT_EQ(result.first[1], -5);
    EXPECT_EQ(result.second[0], -1);
    EXPECT_EQ(result.second[1], -2);
    EXPECT_EQ(result.second[2], -3);
}

TEST(PartitionTest, ContainersWithFloatingPointNumbers)
{
    std::vector<double> c = {1.1, 2.2, -1.1, -2.2, 3.3, -3.3};
    auto result = lbnl::partition(c, [](double x) { return x > 0; });
    EXPECT_EQ(result.first.size(), 3);
    EXPECT_EQ(result.second.size(), 3);
    EXPECT_DOUBLE_EQ(result.first[0], 1.1);
    EXPECT_DOUBLE_EQ(result.first[1], 2.2);
    EXPECT_DOUBLE_EQ(result.first[2], 3.3);
    EXPECT_DOUBLE_EQ(result.second[0], -1.1);
    EXPECT_DOUBLE_EQ(result.second[1], -2.2);
    EXPECT_DOUBLE_EQ(result.second[2], -3.3);
}

TEST(PartitionTest, LargeContainers)
{
    std::vector<int> c(1000);
    for(int i = 0; i < 1000; ++i)
    {
        c[i] = i % 2 == 0 ? -i : i;
    }
    auto result = lbnl::partition(c, [](int x) { return x > 0; });
    EXPECT_EQ(result.first.size(), 500);
    EXPECT_EQ(result.second.size(), 500);
    for(int i = 0; i < 500; ++i)
    {
        EXPECT_EQ(result.first[i], 2 * i + 1);
        EXPECT_EQ(result.second[i], -2 * i);
    }
}