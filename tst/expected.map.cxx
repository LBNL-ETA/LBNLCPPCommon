#include <gtest/gtest.h>
#include <lbnl/expected.hxx>
#include <string>

// Simple function that returns ExpectedExt for testing
lbnl::ExpectedExt<int, std::string> compute(int x) {
    return x > 0 ? lbnl::make_expected<int, std::string>(x) : lbnl::make_unexpected<int, std::string>("non-positive");
}

// Test transform on success
TEST(ExpectedExtTransformTest, Transform_TransformsValue)
{
    auto result = compute(3).transform([](int x) {
        return x * 2;
    });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 6);
}

// Test transform on error — should propagate the same error
TEST(ExpectedExtTransformTest, Transform_SkipsOnError)
{
    auto result = compute(0).transform([](int x) {
        return x * 2;
    });

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "non-positive");
}

// Test transform_error on error
TEST(ExpectedExtTransformTest, TransformError_TransformsError)
{
    auto result = compute(0).transform_error([](const std::string& err) {
        return "ERR: " + err;
    });

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "ERR: non-positive");
}

// Test transform_error on success — should preserve value
TEST(ExpectedExtTransformTest, TransformError_SkipsOnSuccess)
{
    auto result = compute(5).transform_error([](const std::string&) {
        ADD_FAILURE() << "transform_error should not be called";
        return std::string("never");
    });

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);
}
