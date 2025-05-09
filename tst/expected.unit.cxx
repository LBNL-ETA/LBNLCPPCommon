#include <gtest/gtest.h>
#include <lbnl/expected.hxx>
#include <string>

// Helper alias for clarity
using Result = lbnl::ExpectedExt<int, std::string>;

// Test basic success case with operator|
TEST(ExpectedExtTest, OperatorPipe_WithValue)
{
    auto res = lbnl::make_expected<int, std::string>(3) | [](int x) {
        return lbnl::make_expected<int, std::string>(x + 2);
    };

    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 5);
}

// Test chaining multiple operator| calls
TEST(ExpectedExtTest, OperatorPipe_Chained)
{
    auto res = lbnl::make_expected<int, std::string>(2)
        | [](int x) { return lbnl::make_expected<int, std::string>(x * 3); }
        | [](int x) { return lbnl::make_expected<int, std::string>(x + 1); };

    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 7);
}

// Test operator| on an error value — should skip lambda
TEST(ExpectedExtTest, OperatorPipe_WithError)
{
    auto res = lbnl::make_unexpected<int, std::string>("Oops") | [](int x) {
        return lbnl::make_expected<int, std::string>(x + 10);
    };

    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), "Oops");
}

// Test or_else recovers from an error
TEST(ExpectedExtTest, OrElse_RecoverFromError)
{
    auto res = lbnl::make_unexpected<int, std::string>("bad things")
        .or_else([](const std::string& err) {
            EXPECT_EQ(err, "bad things");
            return lbnl::make_expected<int, std::string>(42);
        });

    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 42);
}

// Test or_else is not invoked if value exists
TEST(ExpectedExtTest, OrElse_SkippedOnSuccess)
{
    auto res = lbnl::make_expected<int, std::string>(10)
        .or_else([](const std::string&) {
            ADD_FAILURE() << "or_else should not be called";
            return lbnl::make_expected<int, std::string>(0);
        });

    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 10);
}

// Test and_then with plain value return
TEST(ExpectedExtTest, AndThen_PlainValue)
{
    auto res = lbnl::make_expected<int, std::string>(4)
        .and_then([](int x) { return x * x; });

    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 16);
}

// Test and_then is skipped on error
TEST(ExpectedExtTest, AndThen_SkipOnError)
{
    auto res = lbnl::make_unexpected<int, std::string>("fail")
        .and_then([](int x) {
            ADD_FAILURE() << "and_then should not be called";
            return x + 1;
        });

    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), "fail");
}