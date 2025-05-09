#include <gtest/gtest.h>
#include <lbnl/expected.hxx>
#include <string>

// Simple function that returns ExpectedExt for testing
lbnl::ExpectedExt<int, std::string> compute(int x) {
    return x > 0 ? lbnl::make_expected<int, std::string>(x) : lbnl::make_unexpected<int, std::string>("non-positive");
}

// Test map on success
TEST(ExpectedExtMapTest, Map_TransformsValue)
{
    auto result = compute(3).map([](int x) {
        return x * 2;
    });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 6);
}

// Test map on error — should propagate the same error
TEST(ExpectedExtMapTest, Map_SkipsOnError)
{
    auto result = compute(0).map([](int x) {
        return x * 2;
    });

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "non-positive");
}

// Test map_error on error
TEST(ExpectedExtMapTest, MapError_TransformsError)
{
    auto result = compute(0).map_error([](const std::string& err) {
        return "ERR: " + err;
    });

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "ERR: non-positive");
}

// Test map_error on success — should preserve value
TEST(ExpectedExtMapTest, MapError_SkipsOnSuccess)
{
    auto result = compute(5).map_error([](const std::string&) {
        ADD_FAILURE() << "map_error should not be called";
        return std::string("never");
    });

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);
}