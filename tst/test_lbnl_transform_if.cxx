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

TEST(TransformIfTests, TransformsMatchingElement)
{
    const std::vector<Record> original{
            {"123", "Original A"},
            {"456", "Original B"},
            {"789", "Original C"},
        };

    const Record updatedRecord = {"456", "Updated B"};

    auto result = lbnl::transform_if(original,
                                     [&](const Record & r) { return r.UUID == updatedRecord.UUID; },
                                     [&](const Record &) { return updatedRecord; });

    EXPECT_EQ(result[0].Name, "Original A");
    EXPECT_EQ(result[1].Name, "Updated B");
    EXPECT_EQ(result[2].Name, "Original C");
}

TEST(TransformIfTests, NoMatchReturnsSameElements)
{
    const std::vector<Record> original{
            {"123", "A"},
            {"456", "B"},
        };

    auto result = lbnl::transform_if(original,
                                     [](const Record & r) { return r.UUID == "999"; },
                                     [](const Record & r) { return Record{r.UUID, "ShouldNotChange"}; });

    EXPECT_EQ(result, original);
}

TEST(TransformIfTests, MultipleMatchesAreTransformed)
{
    const std::vector<Record> original{
            {"123", "A"},
            {"123", "A again"},
            {"456", "B"},
        };

    auto result = lbnl::transform_if(original,
                                     [](const Record & r) { return r.UUID == "123"; },
                                     [](const Record & r) { return Record{r.UUID, "Updated"}; });

    EXPECT_EQ(result[0].Name, "Updated");
    EXPECT_EQ(result[1].Name, "Updated");
    EXPECT_EQ(result[2].Name, "B");
}
