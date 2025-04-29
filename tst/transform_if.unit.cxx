#include <gtest/gtest.h>
#include <vector>
#include <string>

#include "lbnl/algorithm.hxx"

struct Record
{
    std::string UUID;
    std::string Name;

    auto operator<=>(const Record &) const = default;
};

TEST(TransformIfTests, EmptyContainerReturnsEmpty)
{
    const std::vector<Record> original{};

    auto result = lbnl::transform_if(
      original,
      [](const Record &) { return true; },
      [](const Record & r) { return Record{r.UUID, "ShouldNotMatter"}; });

    EXPECT_TRUE(result.empty());
}

TEST(TransformIfTests, IdentityTransformationPreservesAll)
{
    const std::vector<Record> original{
      {"123", "A"},
      {"456", "B"},
    };

    auto result = lbnl::transform_if(
      original,
      [](const Record &) { return true; },   // match all
      [](const Record & r) { return r; });   // identity

    EXPECT_EQ(result, original);
}

TEST(TransformIfTests, OriginalUnchangedAfterTransformation)
{
    const std::vector<Record> original{
      {"1", "X"},
      {"2", "Y"},
    };

    auto copy = original;

    auto result = lbnl::transform_if(
      original,
      [](const Record & r) { return r.UUID == "1"; },
      [](const Record &) { return Record{"1", "Z"}; });

    // Ensure the original container is untouched
    EXPECT_EQ(original, copy);
}