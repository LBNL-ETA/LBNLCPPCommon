#include <gtest/gtest.h>

#include <lbnl/optional.hxx>


// Test and_then with a valid optional
TEST(LbnlOptionalTest, AndThen_WithValue)
{
    std::optional opt_value = 5;
    auto result = lbnl::and_then(opt_value, [](int x) { return x * 2; });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 10);
}

// Test and_then with an empty optional
TEST(LbnlOptionalTest, AndThen_Empty)
{
    std::optional<int> opt_value;
    auto result = lbnl::and_then(opt_value, [](int x) { return x * 2; });

    EXPECT_FALSE(result.has_value());
}

// Test or_else with a valid optional
TEST(LbnlOptionalTest, OrElse_WithValue)
{
    std::optional opt_value = 5;
    auto result = lbnl::or_else(opt_value, []() { return 10; });

    EXPECT_EQ(result, 5);   // Should return the original value
}

// Test or_else with an empty optional
TEST(LbnlOptionalTest, OrElse_Empty)
{
    std::optional<int> opt_value;
    auto result = lbnl::or_else(opt_value, []() { return 10; });

    EXPECT_EQ(result, 10);   // Should return the fallback value
}

// Test operator| with a valid optional
TEST(LbnlOptionalTest, OperatorPipe_WithValue)
{
    using namespace lbnl;

    std::optional opt_value = 3;
    auto result = opt_value | [](int x) { return x + 2; };

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);
}

// Test operator| with an empty optional
TEST(LbnlOptionalTest, OperatorPipe_Empty)
{
    using namespace lbnl;

    std::optional<int> opt_value;
    auto result = opt_value | [](int x) { return x + 2; };

    EXPECT_FALSE(result.has_value());
}

// Test operator|| with a valid optional
TEST(LbnlOptionalTest, OperatorOr_WithValue)
{
    using namespace lbnl;

    std::optional opt_value = 7;
    auto result = opt_value || []() { return 20; };

    EXPECT_EQ(result, 7);
}

// Test operator|| with an empty optional
TEST(LbnlOptionalTest, OperatorOr_Empty)
{
    using namespace lbnl;

    std::optional<int> opt_value;
    auto result = opt_value || []() { return 20; };

    EXPECT_EQ(result, 20);
}
