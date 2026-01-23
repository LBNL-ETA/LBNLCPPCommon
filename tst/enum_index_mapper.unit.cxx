#include <optional>

#include <gtest/gtest.h>
#include <lbnl/enum_index_mapper.hxx>

// A tiny dummy enum to validate the generic mapper
enum class SV : int
{
    Top = 1,
    Left = 2,
    Right = 3,
    Bottom = 4
};

// Define a constexpr mapper
inline constexpr auto kSVMap = lbnl::make_enum_index_mapper<SV>({
  {1, SV::Top},
  {2, SV::Left},
  {3, SV::Right},
  {4, SV::Bottom},
});

// ---- Compile-time checks (prove constexpr works) ----
static_assert(kSVMap.to_enum(1).has_value() && *kSVMap.to_enum(1) == SV::Top);
static_assert(kSVMap.to_index(SV::Bottom).has_value() && *kSVMap.to_index(SV::Bottom) == 4);

TEST(EnumIndexMapper_Generic, ToEnum_Valid)
{
    EXPECT_EQ(kSVMap.to_enum(1), SV::Top);
    EXPECT_EQ(kSVMap.to_enum(2), SV::Left);
    EXPECT_EQ(kSVMap.to_enum(3), SV::Right);
    EXPECT_EQ(kSVMap.to_enum(4), SV::Bottom);
}

TEST(EnumIndexMapper_Generic, ToEnum_Invalid)
{
    EXPECT_FALSE(kSVMap.to_enum(0).has_value());
    EXPECT_FALSE(kSVMap.to_enum(5).has_value());
    EXPECT_FALSE(kSVMap.to_enum(-7).has_value());
}

TEST(EnumIndexMapper_Generic, ToIndex_Valid)
{
    EXPECT_EQ(kSVMap.to_index(SV::Top), 1);
    EXPECT_EQ(kSVMap.to_index(SV::Left), 2);
    EXPECT_EQ(kSVMap.to_index(SV::Right), 3);
    EXPECT_EQ(kSVMap.to_index(SV::Bottom), 4);
}

TEST(EnumIndexMapper_Generic, ToIndex_MissingValueInSmallerMap)
{
    // Exercise the "enum not present" path with a smaller mapping
    constexpr auto small = lbnl::make_enum_index_mapper<SV>({
      {1, SV::Top},
      {2, SV::Left},
    });
    EXPECT_FALSE(small.to_index(SV::Right).has_value());
    EXPECT_FALSE(small.to_index(SV::Bottom).has_value());
}

TEST(EnumIndexMapper_Generic, OrHelpers)
{
    EXPECT_EQ(kSVMap.to_enum_or(1, SV::Bottom), SV::Top);
    EXPECT_EQ(kSVMap.to_enum_or(99, SV::Bottom), SV::Bottom);   // fallback path
    EXPECT_EQ(kSVMap.to_index_or(SV::Left, -1), 2);
    EXPECT_EQ(kSVMap.to_index_or(static_cast<SV>(12345), -1),
              -1);   // fallback path (UB if enum invalid in real code, but fine for test)
}

TEST(EnumIndexMapper_Generic, DataExposure)
{
    // Verify stable contents/ordering for data()
    auto const & arr = kSVMap.data();
    ASSERT_EQ(arr.size(), 4u);
    EXPECT_EQ(arr[0].first, 1);
    EXPECT_EQ(arr[0].second, SV::Top);
}
