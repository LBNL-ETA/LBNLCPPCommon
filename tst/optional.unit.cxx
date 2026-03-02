#include <gtest/gtest.h>

#include <lbnl/optional.hxx>


// Test and_then with a valid optional
TEST(OptionalTest, AndThen_WithValue)
{
    constexpr std::optional opt_value{3};
    constexpr auto result = lbnl::extend(opt_value).and_then([](const int x) { return x + 2; });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 5);
}

// Test and_then with an empty optional
TEST(OptionalTest, AndThen_Empty)
{
    constexpr std::optional<int> opt_value;
    constexpr auto result = lbnl::extend(opt_value).and_then([](const int x) { return x + 2; });

    EXPECT_FALSE(result.has_value());
}

// Test or_else with a valid optional (should keep original value)
TEST(OptionalTest, OrElse_WithValue)
{
    constexpr std::optional opt_value = 7;
    constexpr auto result = lbnl::extend(opt_value).or_else([] { return 20; });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 7);
}

// Test or_else with an empty optional (should use fallback)
TEST(OptionalTest, OrElse_Empty)
{
    constexpr std::optional<int> opt_value;
    constexpr auto result = lbnl::extend(opt_value).or_else([] { return 20; });

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 20);
}

// Test value_or with a value present
TEST(OptionalExtTest, ValueOr_WithValue)
{
    constexpr std::optional opt = 10;
    constexpr auto result = lbnl::extend(opt).and_then([](const int x) { return x * 2; }).value_or(0);

    EXPECT_EQ(result, 20);
}

// Test value_or with an empty optional
TEST(OptionalExtTest, ValueOr_Empty)
{
    constexpr std::optional<int> opt;
    constexpr auto result = lbnl::extend(opt).and_then([](const int x) { return x * 2; }).value_or(42);

    EXPECT_EQ(result, 42);
}

// Test value_or directly on OptionalExt (no and_then)
TEST(OptionalExtTest, ValueOr_Direct)
{
    constexpr std::optional opt = 5;
    constexpr auto ext = lbnl::extend(opt);

    EXPECT_EQ(ext.value_or(99), 5);

    constexpr std::optional<int> empty;
    EXPECT_EQ(lbnl::extend(empty).value_or(99), 99);
}

TEST(OptionalExt, AndThen_Void_OnEmpty)
{
    constexpr std::optional<int> o;   // empty
    constexpr auto r = lbnl::extend(o).and_then([](int) noexcept { /* returns void */ });
    // r is OptionalExt<std::monostate>
    EXPECT_FALSE(r.raw().has_value());   // use raw() to reach the underlying std::optional
    // or: EXPECT_EQ(r.value_or(std::monostate{}), std::monostate{});  // also passes
}

TEST(OptionalExt, AndThen_Void_OnValue)
{
    constexpr std::optional o = 42;   // has value
    constexpr auto r = lbnl::extend(o).and_then([](int) noexcept { /* returns void */ });
    EXPECT_TRUE(r.raw().has_value());   // monostate engaged
    // or: EXPECT_EQ(r.value_or(std::monostate{}), std::monostate{});
}
