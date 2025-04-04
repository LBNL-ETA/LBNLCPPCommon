#include <gtest/gtest.h>
#include <optional>
#include <string>

#include <lbnl/optional.hxx>

TEST(OptionalExtTest, AndThenAppliesFunctionWhenPresent)
{
    std::optional<int> opt = 42;

    auto result = lbnl::extend(opt)
        .and_then([](int v) {
            return std::optional<std::string>{ std::to_string(v) };
        });

    EXPECT_TRUE(result.raw().has_value());
    EXPECT_EQ(result.raw().value(), "42");
}

TEST(OptionalExtTest, AndThenSkipsFunctionWhenEmpty)
{
    std::optional<int> opt;

    auto result = lbnl::extend(opt)
        .and_then([](int v) {
            return std::optional<std::string>{ std::to_string(v) };
        });

    EXPECT_FALSE(result.raw().has_value());
}

TEST(OptionalExtTest, OrElseReturnsValueWhenPresent)
{
    std::optional<std::string> opt = "value";

    auto result = lbnl::extend(opt)
        .or_else([] {
            return std::string("fallback");
        });

    EXPECT_EQ(result.raw(), std::optional<std::string>("value"));
}

TEST(OptionalExtTest, OrElseReturnsFallbackWhenEmpty)
{
    std::optional<std::string> opt;

    auto result = lbnl::extend(opt)
        .or_else([] {
            return std::string("fallback");
        });

    EXPECT_EQ(result.raw(), std::optional<std::string>("fallback"));
}

TEST(OptionalExtTest, ChainedAndThenAndOrElseWorks)
{
    std::optional<int> opt = 7;

    auto result = lbnl::extend(opt)
        .and_then([](int v) {
            return std::optional<std::string>{ std::to_string(v * 2) };
        })
        .or_else([] {
            return std::string("none");
        });

    EXPECT_EQ(result.raw(), std::optional<std::string>("14"));
}

TEST(OptionalExtTest, AndThenSupportsVoidReturningFunction)
{
    std::optional<int> opt = 5;
    bool called = false;

    auto result = lbnl::extend(opt)
        .and_then([&](int v) {
            called = true;
            EXPECT_EQ(v, 5);
            // No return
        });

    EXPECT_TRUE(called);
    EXPECT_TRUE(result.raw().has_value());
}

TEST(OptionalExtTest, OrElseSupportsVoidReturningFunction)
{
    std::optional<int> opt;  // empty
    bool fallbackCalled = false;

    auto result = lbnl::extend(opt)
        .or_else([&] {
            fallbackCalled = true;
            // No return
        });

    EXPECT_TRUE(fallbackCalled);
    EXPECT_TRUE(result.raw().has_value());
}
