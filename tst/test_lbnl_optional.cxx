#include <gtest/gtest.h>

#include <lbnl/optional.hxx>


// Test operator| with a valid optional
TEST(OptionalTest, OperatorPipe_WithValue)
{
    using namespace lbnl;

    std::optional opt_value = 3;
    auto result = opt_value | [](int x) { return x + 2; };

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);
}

// Test operator| with an empty optional
TEST(OptionalTest, OperatorPipe_Empty)
{
    using namespace lbnl;

    std::optional<int> opt_value;
    auto result = opt_value | [](int x) { return x + 2; };

    EXPECT_FALSE(result.has_value());
}

// Test operator|| with a valid optional
TEST(OptionalTest, OperatorOr_WithValue)
{
    using namespace lbnl;

    std::optional opt_value = 7;
    auto result = opt_value || []() { return 20; };

    EXPECT_EQ(result, 7);
}

// Test operator|| with an empty optional
TEST(OptionalTest, OperatorOr_Empty)
{
    using namespace lbnl;

    std::optional<int> opt_value;
    auto result = opt_value || []() { return 20; };

    EXPECT_EQ(result, 20);
}
