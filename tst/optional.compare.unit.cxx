#include <compare>
#include <optional>

#include <gtest/gtest.h>

#include <lbnl/optional.hxx>

namespace
{
    [[nodiscard]] lbnl::OptionalExt<int> some(int value)
    {
        return lbnl::extend(std::optional<int>{value});
    }

    [[nodiscard]] lbnl::OptionalExt<int> none()
    {
        return lbnl::extend(std::optional<int>{});
    }
}

TEST(OptionalExtCompare, Equality)
{
    EXPECT_TRUE(some(3) == some(3));
    EXPECT_FALSE(some(3) == some(4));
    EXPECT_TRUE(none() == none());
    EXPECT_TRUE(some(3) != none());
}

TEST(OptionalExtCompare, AgainstNullopt)
{
    EXPECT_TRUE(none() == std::nullopt);
    EXPECT_TRUE(std::nullopt == none());   // reversed
    EXPECT_FALSE(some(3) == std::nullopt);
}

TEST(OptionalExtCompare, AgainstValue)
{
    EXPECT_TRUE(some(7) == 7);
    EXPECT_TRUE(7 == some(7));   // reversed
    EXPECT_FALSE(none() == 7);
}

TEST(OptionalExtCompare, Ordering)
{
    EXPECT_TRUE(none() < some(1));     // empty < engaged
    EXPECT_TRUE(some(1) < some(2));
    EXPECT_TRUE(some(2) > some(1));
    EXPECT_TRUE((some(2) <=> some(2)) == std::strong_ordering::equal);
}

TEST(OptionalExtCompare, OrderingAgainstNulloptAndValue)
{
    EXPECT_FALSE(none() < std::nullopt);   // empty compares equal to nullopt
    EXPECT_TRUE(some(5) > std::nullopt);
    EXPECT_TRUE(some(5) > 4);
    EXPECT_TRUE(none() < 4);               // empty < any value
}
