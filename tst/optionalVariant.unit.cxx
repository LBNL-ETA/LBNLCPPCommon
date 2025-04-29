#include <gtest/gtest.h>

#include <lbnl/optional.hxx>

struct A
{
    int val = 0;
    bool operator==(const A & other) const
    {
        return val == other.val;
    }
};
struct B
{
    std::string name;
    bool operator==(const B & other) const
    {
        return name == other.name;
    }
};
struct C
{};

TEST(GetIfOptTest, ReturnsValueIfTypeMatches)
{
    std::variant<A, B, C> v = A{42};

    auto result = lbnl::get_if_opt<A>(v);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->val, 42);
}

TEST(GetIfOptTest, ReturnsEmptyIfTypeDoesNotMatch)
{
    std::variant<A, B, C> v = B{"hello"};

    auto result = lbnl::get_if_opt<A>(v);

    EXPECT_FALSE(result.has_value());
}

TEST(GetIfOptTest, HandlesAnotherMatchingType)
{
    std::variant<A, B, C> v = B{"test"};

    auto result = lbnl::get_if_opt<B>(v);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->name, "test");
}

TEST(GetIfOptTest, HandlesEmptyType)
{
    std::variant<A, B, C> v = C{};

    auto result = lbnl::get_if_opt<C>(v);

    EXPECT_TRUE(result.has_value());   // should still return something
}

TEST(GetIfOptTest, ChainsWithOptionalExt)
{
    std::variant<A, B, C> v = A{99};

    auto result = lbnl::extend(lbnl::get_if_opt<A>(v))
                    .and_then([](const A & a) { return std::optional<int>{a.val + 1}; })
                    .raw();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 100);
}
