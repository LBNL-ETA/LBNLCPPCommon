#include <gtest/gtest.h>
#include <lbnl/algorithm.hxx>

TEST(UniqueTest, IntegerVectorWithDuplicates) {
    std::vector<int> numbers = {4, 2, 2, 5, 1, 4, 3};
    auto result = lbnl::unique(numbers);

    std::vector<int> expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(result, expected);
}

TEST(UniqueTest, AlreadySortedUnique) {
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    auto result = lbnl::unique(numbers);

    EXPECT_EQ(result, numbers);
}

TEST(UniqueTest, EmptyContainer) {
    std::vector<int> empty_vec;
    auto result = lbnl::unique(empty_vec);

    EXPECT_TRUE(result.empty());
}

TEST(UniqueTest, StringVector) {
    std::vector<std::string> words = {"apple", "banana", "apple", "cherry", "banana"};
    auto result = lbnl::unique(words);

    std::vector<std::string> expected = {"apple", "banana", "cherry"};
    EXPECT_EQ(result, expected);
}

TEST(UniqueTest, AlreadyUniqueElements) {
    std::vector unique_nums = {10, 20, 30, 40, 50};
    auto result = lbnl::unique(unique_nums);

    EXPECT_EQ(result, unique_nums);
}
