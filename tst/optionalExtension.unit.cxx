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

TEST(OptionalExtTest, NestedAndThenIsFlattenedCorrectly)
{
    std::optional<int> opt = 10;

    auto result = lbnl::extend(opt)
        .and_then([](int v) {
            // First step returns optional<string>
            return std::optional<std::string>{"Number: " + std::to_string(v)};
        })
        .and_then([](const std::string& str) {
            // Second step also returns optional based on a condition
            if (str.length() > 5)
            {
                return std::optional<size_t>{str.length()};
            }
            return std::optional<size_t>{};
        });

    EXPECT_TRUE(result.raw().has_value());
    EXPECT_EQ(result.raw().value(), std::string("Number: 10").length());
}

TEST(OptionalExtTest, DeeplyNestedAndThenChainsWork)
{
    std::optional<int> opt = 3;

    auto result = lbnl::extend(opt)
        .and_then([](int v) {
            return std::optional<std::string>{"Level1_" + std::to_string(v)};
        })
        .and_then([](const std::string& s) {
            return s.size();  // plain value
        })
        .and_then([](size_t len) {
            if (len > 5)
                return std::optional<std::string>{"LengthOK"};
            return std::optional<std::string>{};
        });

    EXPECT_TRUE(result.raw().has_value());
    EXPECT_EQ(result.raw().value(), "LengthOK");
}
