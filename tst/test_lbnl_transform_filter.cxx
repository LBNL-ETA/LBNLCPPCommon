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

TEST(TransformFilterTests, EmptyContainerReturnsEmpty)
{
    const std::vector<Record> original{};

    auto result = lbnl::transform_filter(
      original,
      [](const Record &) { return true; },
      [](const Record & r) { return r.UUID; });   // transform to UUID string

    EXPECT_TRUE(result.empty());
}

TEST(TransformFilterTests, NoElementsPassingPredicateReturnsEmpty)
{
    const std::vector<Record> original{
          {"123", "A"},
          {"456", "B"},
        };

    auto result = lbnl::transform_filter(
      original,
      [](const Record &) { return false; },   // match none
      [](const Record & r) { return r.UUID; });

    EXPECT_TRUE(result.empty());
}

TEST(TransformFilterTests, OnlyMatchingElementsTransformed)
{
    const std::vector<Record> original{
          {"123", "A"},
          {"456", "B"},
          {"789", "C"},
        };

    auto result = lbnl::transform_filter(
      original,
      [](const Record & r) { return r.UUID != "456"; },   // skip UUID "456"
      [](const Record & r) { return r.Name; });           // extract Name

    std::vector<std::string> expected = {"A", "C"};
    EXPECT_EQ(result, expected);
}

TEST(TransformFilterTests, OriginalUnchangedAfterTransformation)
{
    const std::vector<Record> original{
          {"1", "X"},
          {"2", "Y"},
        };

    auto copy = original;

    auto result = lbnl::transform_filter(
      original,
      [](const Record & r) { return r.UUID == "1"; },
      [](const Record & r) { return r.Name; });

    // Ensure the original container is untouched
    EXPECT_EQ(original, copy);

    // Check that the result contains only the transformed value for UUID "1"
    std::vector<std::string> expected{"X"};
    EXPECT_EQ(result, expected);
}