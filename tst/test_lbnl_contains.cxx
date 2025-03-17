#include <gtest/gtest.h>

#include <lbnl/algorithm.hxx>

TEST(ContainsTest, IntegerVector) {
    std::vector numbers = {1, 2, 3, 4, 5};

    EXPECT_TRUE(lbnl::contains(numbers, 3));
    EXPECT_TRUE(lbnl::contains(numbers, 1));
    EXPECT_TRUE(lbnl::contains(numbers, 5));
    EXPECT_FALSE(lbnl::contains(numbers, 10));
    EXPECT_FALSE(lbnl::contains(numbers, -1));
}

TEST(ContainsTest, StringVector) {
    std::vector<std::string> words = {"apple", "banana", "cherry"};

    EXPECT_TRUE(lbnl::contains(words, "banana"));
    EXPECT_FALSE(lbnl::contains(words, "grape"));
}

TEST(ContainsTest, EmptyContainer) {
    std::vector<int> empty_vec;
    EXPECT_FALSE(lbnl::contains(empty_vec, 1));

    std::vector<std::string> empty_str_vec;
    EXPECT_FALSE(lbnl::contains(empty_str_vec, "test"));
}

TEST(ContainsTest, SetContainer) {
    std::set num_set = {10, 20, 30, 40};

    EXPECT_TRUE(lbnl::contains(num_set, 20));
    EXPECT_FALSE(lbnl::contains(num_set, 25));
}

TEST(ContainsTest, StringViewVector) {
    std::vector<std::string_view> str_views = {"foo", "bar", "baz"};

    EXPECT_TRUE(lbnl::contains(str_views, "foo"));
    EXPECT_FALSE(lbnl::contains(str_views, "qux"));
}
