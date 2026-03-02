#include <gtest/gtest.h>
#include <string>

#include <lbnl/expected.hxx>

// Helper alias for clarity
using Result = lbnl::ExpectedExt<int, std::string>;

// Test and_then with a value returning ExpectedExt
TEST(ExpectedExtTest, AndThen_WithExpectedReturn)
{
    auto res = lbnl::make_expected<int, std::string>(3)
                 .and_then([](int x) { return lbnl::make_expected<int, std::string>(x + 2); });

    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 5);
}

// Test chaining multiple and_then calls
TEST(ExpectedExtTest, AndThen_Chained)
{
    auto res = lbnl::make_expected<int, std::string>(2)
                 .and_then([](int x) { return lbnl::make_expected<int, std::string>(x * 3); })
                 .and_then([](int x) { return lbnl::make_expected<int, std::string>(x + 1); });

    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 7);
}

// Test and_then on an error value — should skip lambda
TEST(ExpectedExtTest, AndThen_WithError_Expected)
{
    auto res = lbnl::make_unexpected<int, std::string>("Oops")
                 .and_then([](int x) { return lbnl::make_expected<int, std::string>(x + 10); });

    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), "Oops");
}

// Test or_else recovers from an error
TEST(ExpectedExtTest, OrElse_RecoverFromError)
{
    auto res =
      lbnl::make_unexpected<int, std::string>("bad things").or_else([](const std::string & err) {
          EXPECT_EQ(err, "bad things");
          return lbnl::make_expected<int, std::string>(42);
      });

    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 42);
}

// Test or_else is not invoked if value exists
TEST(ExpectedExtTest, OrElse_SkippedOnSuccess)
{
    auto res = lbnl::make_expected<int, std::string>(10).or_else([](const std::string &) {
        ADD_FAILURE() << "or_else should not be called";
        return lbnl::make_expected<int, std::string>(0);
    });

    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 10);
}

// Test transform with plain value return
TEST(ExpectedExtTest, Transform_PlainValue)
{
    auto res = lbnl::make_expected<int, std::string>(4).transform([](int x) { return x * x; });

    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 16);
}

// Test transform is skipped on error
TEST(ExpectedExtTest, Transform_SkipOnError)
{
    auto res = lbnl::make_unexpected<int, std::string>("fail").transform([](int x) {
        ADD_FAILURE() << "transform should not be called";
        return x + 1;
    });

    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), "fail");
}

// ============================================================
// Tests for ExpectedExt<T, E> where T == E (same type variant)
// ============================================================
using SameType = lbnl::ExpectedExt<std::string, std::string>;

TEST(ExpectedExtSameType, ConstructFromValue)
{
    SameType res{std::string("hello")};
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), "hello");
}

TEST(ExpectedExtSameType, ConstructFromUnexpected)
{
    SameType res{lbnl::Unexpected<std::string>("oops")};
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), "oops");
}

TEST(ExpectedExtSameType, MakeExpected)
{
    auto res = lbnl::make_expected<std::string, std::string>("success");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), "success");
}

TEST(ExpectedExtSameType, MakeUnexpected)
{
    auto res = lbnl::make_unexpected<std::string, std::string>("failure");
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), "failure");
}

TEST(ExpectedExtSameType, BoolConversion)
{
    auto val = lbnl::make_expected<std::string, std::string>("ok");
    auto err = lbnl::make_unexpected<std::string, std::string>("bad");
    EXPECT_TRUE(static_cast<bool>(val));
    EXPECT_FALSE(static_cast<bool>(err));
}

TEST(ExpectedExtSameType, ValueOr)
{
    auto val = lbnl::make_expected<std::string, std::string>("real");
    auto err = lbnl::make_unexpected<std::string, std::string>("bad");
    EXPECT_EQ(val.value_or("fallback"), "real");
    EXPECT_EQ(err.value_or("fallback"), "fallback");
}

TEST(ExpectedExtSameType, Transform_WithValue)
{
    auto res = lbnl::make_expected<std::string, std::string>("input")
                 .transform([](const std::string & str) { return str + "_modified"; });
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), "input_modified");
}

TEST(ExpectedExtSameType, Transform_WithError)
{
    auto res = lbnl::make_unexpected<std::string, std::string>("err")
                 .transform([](const std::string & str) { return str + "_modified"; });
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), "err");
}

TEST(ExpectedExtSameType, OrElse_RecoverFromError)
{
    auto res = lbnl::make_unexpected<std::string, std::string>("bad")
                 .or_else([](const std::string & err) {
                     return lbnl::make_expected<std::string, std::string>("recovered_" + err);
                 });
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), "recovered_bad");
}

TEST(ExpectedExtSameType, OrElse_SkippedOnSuccess)
{
    auto res = lbnl::make_expected<std::string, std::string>("good")
                 .or_else([](const std::string &) {
                     ADD_FAILURE() << "or_else should not be called";
                     return lbnl::make_expected<std::string, std::string>("bad");
                 });
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), "good");
}

TEST(ExpectedExtSameType, Transform_Value_Mapped)
{
    auto res = lbnl::make_expected<std::string, std::string>("data")
                 .transform([](const std::string & str) { return str + "_transformed"; });
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), "data_transformed");
}

TEST(ExpectedExtSameType, Transform_Error_Skipped)
{
    auto res = lbnl::make_unexpected<std::string, std::string>("err")
                 .transform([](const std::string & str) { return str + "_transformed"; });
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), "err");
}

TEST(ExpectedExtSameType, TransformError_WithError)
{
    auto res = lbnl::make_unexpected<std::string, std::string>("err")
                 .transform_error([](const std::string & err) { return "wrapped_" + err; });
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), "wrapped_err");
}

TEST(ExpectedExtSameType, TransformError_WithValue)
{
    auto res = lbnl::make_expected<std::string, std::string>("val")
                 .transform_error([](const std::string & err) { return "wrapped_" + err; });
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), "val");
}

TEST(ExpectedExtSameType, Transform_Chained)
{
    auto res = lbnl::make_expected<std::string, std::string>("start")
                 .transform([](const std::string & str) { return str + "_first"; })
                 .transform([](const std::string & str) { return str + "_second"; });
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), "start_first_second");
}

TEST(ExpectedExtSameType, Transform_Chained_Error)
{
    auto res = lbnl::make_unexpected<std::string, std::string>("fail")
                 .transform([](const std::string & str) { return str + "_first"; })
                 .transform([](const std::string & str) { return str + "_second"; });
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), "fail");
}
