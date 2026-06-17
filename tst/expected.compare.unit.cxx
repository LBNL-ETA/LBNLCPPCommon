#include <gtest/gtest.h>
#include <string>

#include <lbnl/expected.hxx>

namespace
{
    [[nodiscard]] lbnl::ExpectedExt<int, std::string> ok(int value)
    {
        return lbnl::make_expected<int, std::string>(value);
    }

    [[nodiscard]] lbnl::ExpectedExt<int, std::string> err(std::string message)
    {
        return lbnl::make_unexpected<int, std::string>(std::move(message));
    }
}

TEST(ExpectedExtCompare, ValueEquality)
{
    EXPECT_TRUE(ok(5) == ok(5));
    EXPECT_FALSE(ok(5) == ok(6));
    EXPECT_TRUE(ok(5) != ok(6));
}

TEST(ExpectedExtCompare, ErrorEquality)
{
    EXPECT_TRUE(err("e") == err("e"));
    EXPECT_FALSE(err("e") == err("f"));
}

TEST(ExpectedExtCompare, ValueVsErrorNeverEqual)
{
    EXPECT_FALSE(ok(5) == err("e"));
    EXPECT_TRUE(ok(5) != err("e"));
}

TEST(ExpectedExtCompare, CompareWithBareValue)
{
    EXPECT_TRUE(ok(5) == 5);
    EXPECT_TRUE(5 == ok(5));   // reversed, synthesized by C++20
    EXPECT_TRUE(ok(5) != 6);
    EXPECT_FALSE(err("e") == 5);
}

TEST(ExpectedExtCompare, CompareWithUnexpected)
{
    EXPECT_TRUE(err("boom") == lbnl::Unexpected<std::string>("boom"));
    EXPECT_TRUE(lbnl::Unexpected<std::string>("boom") == err("boom"));   // reversed
    EXPECT_FALSE(ok(5) == lbnl::Unexpected<std::string>("boom"));
}
