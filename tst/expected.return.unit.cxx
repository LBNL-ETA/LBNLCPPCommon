#include <gtest/gtest.h>
#include <lbnl/expected.hxx>
#include <string>

// Safe division function with error handling
lbnl::ExpectedExt<int, std::string> safe_divide(int a, int b) {
    if (b == 0) {
        return lbnl::make_unexpected<int, std::string>("Division by zero");
    }
    return lbnl::make_expected<int, std::string>(a / b);
}

// Adds 5 to a number, no error
lbnl::ExpectedExt<int, std::string> add_five(int x) {
    return lbnl::make_expected<int, std::string>(x + 5);
}

// Converts a value to string (pure transformation)
std::string to_string(int x) {
    return std::to_string(x);
}

// Test direct use of a function returning ExpectedExt
TEST(ExpectedExtReturnTest, SafeDivide_Success)
{
    auto result = safe_divide(10, 2);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);
}

TEST(ExpectedExtReturnTest, SafeDivide_Error)
{
    auto result = safe_divide(10, 0);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Division by zero");
}

// Test chaining two functions using and_then
TEST(ExpectedExtReturnTest, Chain_SafeDivide_AddFive)
{
    auto result = safe_divide(20, 4)
        .and_then(add_five);  // 20/4 = 5 → 5+5 = 10

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 10);
}

// Test chaining fails on divide by zero
TEST(ExpectedExtReturnTest, Chain_WithError_SkipsNextStep)
{
    auto result = safe_divide(10, 0)
        .and_then(add_five);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Division by zero");
}

// Test fallback recovery with or_else
TEST(ExpectedExtReturnTest, Fallback_OnError)
{
    auto result = safe_divide(10, 0)
        .or_else([](const std::string& err) {
            EXPECT_EQ(err, "Division by zero");
            return lbnl::make_expected<int, std::string>(-999);
        });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), -999);
}

// Test chaining + fallback
TEST(ExpectedExtReturnTest, ChainAndFallback)
{
    auto result = safe_divide(10, 0)
        .and_then(add_five)
        .or_else([](const std::string&) {
            return lbnl::make_expected<int, std::string>(0);
        });

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);
}