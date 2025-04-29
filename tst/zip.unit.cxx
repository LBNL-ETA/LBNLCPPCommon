#include <gtest/gtest.h>
#include <vector>
#include <utility>

#include <lbnl/algorithm.hxx>

TEST(ZipTest, EmptyContainers) {
    std::vector<int> c1;
    std::vector<int> c2;
    auto result = lbnl::zip(c1, c2);
    EXPECT_TRUE(result.empty());
}

TEST(ZipTest, UnequalSizeContainers) {
    std::vector<int> c1 = {1, 2, 3};
    std::vector<int> c2 = {4, 5};
    auto result = lbnl::zip(c1, c2);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], std::make_pair(1, 4));
    EXPECT_EQ(result[1], std::make_pair(2, 5));
}

TEST(ZipTest, EqualSizeContainers) {
    std::vector<int> c1 = {1, 2, 3};
    std::vector<int> c2 = {4, 5, 6};
    auto result = lbnl::zip(c1, c2);
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], std::make_pair(1, 4));
    EXPECT_EQ(result[1], std::make_pair(2, 5));
    EXPECT_EQ(result[2], std::make_pair(3, 6));
}

TEST(ZipTest, DifferentTypeContainers) {
    std::vector<int> c1 = {1, 2, 3};
    std::vector<double> c2 = {4.5, 5.5, 6.5};
    auto result = lbnl::zip(c1, c2);
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], std::make_pair(1, 4.5));
    EXPECT_EQ(result[1], std::make_pair(2, 5.5));
    EXPECT_EQ(result[2], std::make_pair(3, 6.5));
}

TEST(ZipTest, LargeContainers) {
    std::vector<int> c1(1000);
    std::vector<int> c2(1000);
    for (int i = 0; i < 1000; ++i) {
        c1[i] = i;
        c2[i] = i * 2;
    }
    auto result = lbnl::zip(c1, c2);
    EXPECT_EQ(result.size(), 1000);
    for (int i = 0; i < 1000; ++i) {
        EXPECT_EQ(result[i], std::make_pair(i, i * 2));
    }
}