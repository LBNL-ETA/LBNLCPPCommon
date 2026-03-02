// expected.type_stability.unit.cxx
#include <gtest/gtest.h>
#include <type_traits>
#include <string>
#include <cmath>

#include <lbnl/expected.hxx>

// Mapper #1: int -> ExpectedExt<double, std::string>
static auto to_double_expected = [](const int x) {
    return lbnl::make_expected<double, std::string>(static_cast<double>(x) * 1.5);
};

// Mapper #2: int -> double (plain value, for use with transform)
static auto to_double_plain = [](const int x) { return static_cast<double>(x) * 1.5; };

TEST(ExpectedExt_AndThen, TypeChange_ErrorPath_KeepsE)
{
    auto err = lbnl::make_unexpected<int, std::string>(std::string{"boom"});
    auto r = err.and_then(to_double_expected);

    // Result type must change T=int -> U=double, E stays std::string
    static_assert(std::is_same_v<decltype(r), lbnl::ExpectedExt<double, std::string>>);

    EXPECT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), "boom");
}

TEST(ExpectedExt_AndThen, TypeChange_SuccessPath_MapsToU)
{
    const auto ok = lbnl::make_expected<int, std::string>(8);
    auto r = ok.and_then(to_double_expected);

    static_assert(std::is_same_v<decltype(r), lbnl::ExpectedExt<double, std::string>>);

    ASSERT_TRUE(r.has_value());
    EXPECT_DOUBLE_EQ(r.value(), 12.0);   // 8 * 1.5
}

TEST(ExpectedExt_Transform, PlainValueFunc_WrappedIntoExpected)
{
    // Error path: should preserve error and still be ExpectedExt<double, std::string>
    const auto err = lbnl::make_unexpected<int, std::string>(std::string{"e"});
    auto r1 = err.transform(to_double_plain);
    static_assert(std::is_same_v<decltype(r1), lbnl::ExpectedExt<double, std::string>>);
    EXPECT_FALSE(r1.has_value());
    EXPECT_EQ(r1.error(), "e");

    // Success path: value gets wrapped into ExpectedExt<double, std::string>
    const auto ok = lbnl::make_expected<int, std::string>(4);
    auto r2 = ok.transform(to_double_plain);
    static_assert(std::is_same_v<decltype(r2), lbnl::ExpectedExt<double, std::string>>);
    ASSERT_TRUE(r2.has_value());
    EXPECT_DOUBLE_EQ(r2.value(), 6.0);
}

TEST(ExpectedExt_Transform, TransformsToDifferentT)
{
    const auto ok = lbnl::make_expected<int, std::string>(5);
    auto r = ok.transform([](int x) { return x * 2.0; });   // T: int -> double

    static_assert(std::is_same_v<decltype(r), lbnl::ExpectedExt<double, std::string>>);
    ASSERT_TRUE(r.has_value());
    EXPECT_DOUBLE_EQ(r.value(), 10.0);

    const auto err = lbnl::make_unexpected<int, std::string>(std::string{"x"});
    auto r2 = err.transform([](int x) { return x * 2.0; });
    EXPECT_FALSE(r2.has_value());
    EXPECT_EQ(r2.error(), "x");
}

TEST(ExpectedExt_TransformError, TransformsError_PreservesValue)
{
    const auto err = lbnl::make_unexpected<int, std::string>(std::string{"oops"});
    auto r = err.transform_error([](const std::string & e) { return e.size(); });   // E -> size_t

    static_assert(std::is_same_v<decltype(r), lbnl::ExpectedExt<int, std::size_t>>);
    EXPECT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), std::string("oops").size());

    const auto ok = lbnl::make_expected<int, std::string>(9);
    auto r2 = ok.transform_error([](const std::string & e) { return e.size(); });
    EXPECT_TRUE(r2.has_value());
    EXPECT_EQ(r2.value(), 9);
}

TEST(ExpectedExt_AndThen, ChainsTransformThenAndThen)
{
    auto r = lbnl::make_expected<int, std::string>(3)
               .transform([](int x) { return x + 1; })
               .and_then([](int x) { return lbnl::make_expected<double, std::string>(x * 2.5); });

    static_assert(std::is_same_v<decltype(r), lbnl::ExpectedExt<double, std::string>>);
    ASSERT_TRUE(r.has_value());
    EXPECT_DOUBLE_EQ(r.value(), 10.0);   // (3+1)*2.5
}

TEST(ExpectedExt_Transform, TransformsValue_PreservesErrorType)
{
    const auto ok = lbnl::make_expected<int, std::string>(3);
    auto r = ok.transform([](int x) { return x * 2.0; });   // int -> double

    static_assert(std::is_same_v<decltype(r), lbnl::ExpectedExt<double, std::string>>);
    ASSERT_TRUE(r.has_value());
    EXPECT_DOUBLE_EQ(r.value(), 6.0);
}

TEST(ExpectedExt_Transform, ValueFunc_WrappedIntoExpected)
{
    const auto ok = lbnl::make_expected<int, std::string>(4);
    auto r = ok.transform([](int x) { return x * 1.5; });   // int -> double (plain value)
    static_assert(std::is_same_v<decltype(r), lbnl::ExpectedExt<double, std::string>>);
    ASSERT_TRUE(r.has_value());
    EXPECT_DOUBLE_EQ(r.value(), 6.0);
}

TEST(ExpectedExt_TransformError, TransformsErrorType)
{
    const auto err = lbnl::make_unexpected<int, std::string>("oops");
    auto r = err.transform_error([](const std::string & e) { return e.size(); });   // E: string -> size_t
    static_assert(std::is_same_v<decltype(r), lbnl::ExpectedExt<int, std::size_t>>);
    EXPECT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), std::string("oops").size());
}
