#include <gtest/gtest.h>

#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <set>

#include "lbnl/memoize.hxx"

TEST(LazyEvaluatorTests, ReturnsCorrectValue)
{
    std::atomic callCount{0};

    // Simulate expensive function
    lbnl::LazyEvaluator<int, std::string> lazyEval([&callCount](int key) {
        ++callCount;
        return std::to_string(key * 10);
    });

    // First call should evaluate
    EXPECT_EQ(lazyEval(2), "20");
    EXPECT_EQ(callCount, 1);

    // Second call with same key should NOT re-evaluate
    EXPECT_EQ(lazyEval(2), "20");
    EXPECT_EQ(callCount, 1);

    // Different key triggers evaluation
    EXPECT_EQ(lazyEval(3), "30");
    EXPECT_EQ(callCount, 2);

    // Again same key, no re-eval
    EXPECT_EQ(lazyEval(3), "30");
    EXPECT_EQ(callCount, 2);
}

struct Person
{
    std::string firstName;
    std::string lastName;
    int birthYear;

    bool operator==(const Person & other) const
    {
        return firstName == other.firstName && lastName == other.lastName
               && birthYear == other.birthYear;
    }
};

namespace std
{
    template<>
    struct hash<Person>
    {
        std::size_t operator()(const Person & p) const
        {
            std::size_t h1 = std::hash<std::string>{}(p.firstName);
            std::size_t h2 = std::hash<std::string>{}(p.lastName);
            std::size_t h3 = std::hash<int>{}(p.birthYear);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}   // namespace std

TEST(LazyEvaluatorTests, WorksWithPersonStruct)
{
    std::atomic<int> callCount{0};

    lbnl::LazyEvaluator<Person, std::string> lazyEval([&callCount](const Person & p) {
        ++callCount;
        return p.firstName + " " + p.lastName + " (" + std::to_string(p.birthYear) + ")";
    });

    Person alice{"Alice", "Smith", 1985};
    Person bob{"Bob", "Johnson", 1990};
    Person aliceClone{"Alice", "Smith", 1985};   // logically same as `alice`

    // First call for Alice
    EXPECT_EQ(lazyEval(alice), "Alice Smith (1985)");
    EXPECT_EQ(callCount, 1);

    // First call for Bob
    EXPECT_EQ(lazyEval(bob), "Bob Johnson (1990)");
    EXPECT_EQ(callCount, 2);

    // Cache hit for Alice
    EXPECT_EQ(lazyEval(aliceClone), "Alice Smith (1985)");
    EXPECT_EQ(callCount, 2);   // no new computation
}

TEST(LazyEvaluatorTest, ParallelAccessSameKey)
{
    using namespace std::chrono_literals;

    std::atomic computeCount{0};
    lbnl::LazyEvaluator<int, std::string> evaluator([&](int key) {
        ++computeCount;
        std::this_thread::sleep_for(50ms);   // Simulate expensive work
        return "Value_" + std::to_string(key);
    });

    constexpr int sharedKey = 123;
    constexpr int numThreads = 8;

    std::vector<std::thread> threads;
    std::vector<std::string> results(numThreads);

    for(int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(
          [&evaluator, &results, i]() { results[i] = evaluator.get(sharedKey); });
    }

    for(auto & t : threads)
    {
        t.join();
    }

    std::set uniqueResults(results.begin(), results.end());
    ASSERT_EQ(uniqueResults.size(), 1);
    ASSERT_EQ(*uniqueResults.begin(), "Value_123");

    // Only one computation should have occurred
    ASSERT_EQ(computeCount.load(), 1);
}