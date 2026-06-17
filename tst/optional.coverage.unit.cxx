#include <compare>
#include <optional>

#include <gtest/gtest.h>

#include <lbnl/optional.hxx>

// Targets paths the other optional tests miss (confirmed via gcov on the
// coverage build): the and_then "empty optional" branch for void- and
// plain-value-returning callbacks (detail::to_ext_empty), the or_else
// void-callback-skipped-when-present branch, and comparison-operator edges.

TEST(OptionalExtCoverage, AndThenEmptyWithVoidFunc)
{
    // Empty + void callback: callback is skipped and to_ext_empty<void> runs.
    bool called = false;
    const auto result = lbnl::extend(std::optional<int>{}).and_then([&](int) { called = true; });
    EXPECT_FALSE(called);
    EXPECT_FALSE(result.raw().has_value());
}

TEST(OptionalExtCoverage, AndThenEmptyWithPlainValueFunc)
{
    // Empty + plain-value callback: to_ext_empty<int> yields an empty OptionalExt<int>.
    const auto result = lbnl::extend(std::optional<int>{}).and_then([](int val) { return val * 2; });
    EXPECT_FALSE(result.raw().has_value());
}

TEST(OptionalExtCoverage, AndThenEmptyWithOptionalExtFunc)
{
    // Empty + OptionalExt-returning callback: to_ext_empty hits the is_optional_ext branch.
    const auto result = lbnl::extend(std::optional<int>{})
                          .and_then([](int val) { return lbnl::extend(std::optional<int>{val}); });
    EXPECT_FALSE(result.raw().has_value());
}

TEST(OptionalExtCoverage, OrElseVoidFallbackSkippedWhenPresent)
{
    // Present + void fallback: the fallback must NOT run (the !m_opt false branch).
    bool called = false;
    const auto result = lbnl::extend(std::optional<int>{9}).or_else([&] { called = true; });
    EXPECT_FALSE(called);
    EXPECT_TRUE(result.raw().has_value());
}

TEST(OptionalExtCoverage, EqualityPresentButUnequal)
{
    // has_value() true but values differ -> exercises the *lhs == val false branch.
    EXPECT_FALSE(lbnl::extend(std::optional<int>{7}) == 8);
    EXPECT_FALSE(lbnl::extend(std::optional<int>{7}) == lbnl::extend(std::optional<int>{8}));
}

TEST(OptionalExtCoverage, OrderingPresentVsAbsentAndValue)
{
    // opt <=> opt with lhs present, rhs empty (the "not both engaged" branch).
    EXPECT_TRUE(lbnl::extend(std::optional<int>{1}) > lbnl::extend(std::optional<int>{}));
    // opt <=> value, all three orderings.
    EXPECT_TRUE(lbnl::extend(std::optional<int>{3}) < 4);
    EXPECT_TRUE(lbnl::extend(std::optional<int>{4}) > 3);
    EXPECT_TRUE((lbnl::extend(std::optional<int>{5}) <=> 5) == std::strong_ordering::equal);
}
