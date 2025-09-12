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

// Test value_or with a value present
TEST(OptionalExtTest, ValueOr_WithValue)
{
    using namespace lbnl;

    std::optional<int> opt = 10;
    auto result = extend(opt).and_then([](int x) { return x * 2; }).value_or(0);

    EXPECT_EQ(result, 20);
}

// Test value_or with an empty optional
TEST(OptionalExtTest, ValueOr_Empty)
{
    using namespace lbnl;

    std::optional<int> opt;
    auto result = extend(opt).and_then([](int x) { return x * 2; }).value_or(42);

    EXPECT_EQ(result, 42);
}

// Test value_or directly on OptionalExt (no and_then)
TEST(OptionalExtTest, ValueOr_Direct)
{
    using namespace lbnl;

    std::optional<int> opt = 5;
    auto ext = extend(opt);

    EXPECT_EQ(ext.value_or(99), 5);

    std::optional<int> empty;
    EXPECT_EQ(extend(empty).value_or(99), 99);
}

TEST(OptionalExt, AndThen_Void_OnEmpty)
{
    std::optional<int> o;   // empty
    auto r = lbnl::extend(o).and_then([](int) noexcept { /* returns void */ });
    // r is OptionalExt<std::monostate>
    EXPECT_FALSE(r.raw().has_value());   // use raw() to reach the underlying std::optional
    // or: EXPECT_EQ(r.value_or(std::monostate{}), std::monostate{});  // also passes
}

TEST(OptionalExt, AndThen_Void_OnValue)
{
    std::optional<int> o = 42;   // has value
    auto r = lbnl::extend(o).and_then([](int) noexcept { /* returns void */ });
    EXPECT_TRUE(r.raw().has_value());   // monostate engaged
    // or: EXPECT_EQ(r.value_or(std::monostate{}), std::monostate{});
}