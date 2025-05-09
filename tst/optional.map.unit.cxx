#include <gtest/gtest.h>
#include <lbnl/optional.hxx>
#include <optional>
#include <string>

TEST(OptionalExtMapTest, Map_TransformsValue)
{
    std::optional<int> opt = 10;
    auto result = lbnl::extend(opt).map([](int x) {
        return std::to_string(x);
    });

    ASSERT_TRUE(result.raw().has_value());
    EXPECT_EQ(result.raw().value(), "10");
}

TEST(OptionalExtMapTest, Map_SkipsOnEmpty)
{
    std::optional<int> opt;
    auto result = lbnl::extend(opt).map([](int x) {
        return x * 2;
    });

    EXPECT_FALSE(result.raw().has_value());
}

TEST(OptionalExtMapTest, Transform_EqualsMap)
{
    std::optional<int> opt = 5;

    auto map_result = lbnl::extend(opt).map([](int x) { return x + 1; });
    auto transform_result = lbnl::extend(opt).transform([](int x) { return x + 1; });

    ASSERT_TRUE(map_result.raw().has_value());
    ASSERT_TRUE(transform_result.raw().has_value());
    EXPECT_EQ(map_result.raw().value(), transform_result.raw().value());
}