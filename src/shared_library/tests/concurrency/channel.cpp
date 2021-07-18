#include <future>
#include <array>
#include <vector>
#include <cstdint>
#include <atomic>
#include <execution>
#include <algorithm>
#include <mutex>

#include "catch2/catch.hpp"
#include "concurrency/channel.h"

using namespace std::literals;
using namespace projectfarm::shared::concurrency;

/*********************************************
 * Push
 ********************************************/

TEST_CASE("Push - push value on single thread - pushes value", "[concurrency]")
{
    auto expected = 1;

    channel<uint32_t> c;

    c.Push(expected);

    auto result = c.GetAll();

    REQUIRE(result.size() == 1);
    REQUIRE(result[0] == expected);
}

TEST_CASE("Push - push value on multiple threads - pushes values", "[concurrency]")
{
    std::vector<uint32_t> expected { 1,2,3,4,5,6 };

    channel<uint32_t> c;

    std::vector<std::future<void>> futures;

    for (const auto& v : expected)
    {
        auto future = std::async(std::launch::async, [&c, v](){ c.Push(v); });
        futures.emplace_back(std::move(future));
    }

    for (auto& f : futures)
    {
        f.wait();
    }

    auto result = c.GetAll();

    REQUIRE(result.size() == expected.size());

    // sort to make comparing easier
    std::sort(result.begin(), result.end());

    REQUIRE(result == expected);
}

/*********************************************
 * GetAll
 ********************************************/

TEST_CASE("GetAll - get values on single thread - gets all values", "[concurrency]")
{
    std::vector<uint32_t> expected { 1,2,3,4,5,6 };

    channel<uint32_t> c;

    for (const auto& v : expected)
    {
        c.Push(v);
    }

    auto result = c.GetAll();

    REQUIRE(result.size() == expected.size());
    REQUIRE(result == expected);
}

TEST_CASE("GetAll - empties channel after call - channel is has no values", "[concurrency]")
{
    std::vector<uint32_t> expected { 1,2,3,4,5,6 };

    channel<uint32_t> c;

    for (const auto& v : expected)
    {
        c.Push(v);
    }

    auto result = c.HasValues();

    REQUIRE(result);

    auto values = c.GetAll();

    result = c.HasValues();

    REQUIRE_FALSE(result);
}

TEST_CASE("GetAll - called on multiple threads - only the first call gets the values", "[concurrency]")
{
    std::vector<uint32_t> expected { 1,2,3,4,5,6 };
    std::atomic_bool hasGotValue {false};

    channel<uint32_t> c;

    for (const auto& v : expected)
    {
        c.Push(v);
    }

    auto f = [&c, &hasGotValue, expected]()
    {
        auto values = c.GetAll();

        if (hasGotValue)
        {
            REQUIRE(values.empty());
        }
        else
        {
            REQUIRE(values == expected);

            hasGotValue = true;
        }
    };

    std::vector<std::future<void>> futures;
    for (auto i {0u}; i < 5; ++i)
    {
        auto future = std::async(std::launch::async, f);
        futures.emplace_back(std::move(future));
    }
}

TEST_CASE("GetAll - gets values pushed on multiple threads - streams all pushed values", "[concurrency]")
{
    std::vector<uint32_t> expected(1000);

    // we will pull all values into this array
    constexpr auto resultBucketCount {3u};
    std::array<std::vector<uint32_t>, resultBucketCount> results;

    channel<uint32_t> c;

    // fill from 0 to expected.size() - 1
    std::iota(expected.begin(), expected.end(), 0);

    auto pushFunc = [&c, expected](auto from, auto to)
    {
        for (auto i {from}; i <= to; ++i)
        {
            c.Push(expected[i]);
        }
    };

    std::mutex mutex;
    auto getFunc = [&c, &results, &mutex](auto index, auto expectedSize)
    {
        while (true)
        {
            auto values = c.GetAll();

            std::scoped_lock lock(mutex);

            results[index].insert(results[index].end(), values.begin(), values.end());

            auto count {0u};
            for (const auto& res : results)
            {
                count += res.size();
            }

            if (count == expectedSize)
            {
                break;
            }
        }
    };

    std::vector<std::future<void>> futures;
    futures.emplace_back(std::async(std::launch::async,
                                    [&getFunc, &expected](){ getFunc(0, expected.size()); }));
    futures.emplace_back(std::async(std::launch::async,
                                    [&getFunc, &expected](){ getFunc(1, expected.size()); }));
    futures.emplace_back(std::async(std::launch::async,
                                    [&getFunc, &expected](){ getFunc(2, expected.size()); }));

    // clang doesn't support this yet...
    //std::for_each(std::execution::par_unseq, expected.begin(), expected.end(), pushFunc);
    std::async(std::launch::async, [&pushFunc](){ pushFunc(0, 299); });
    std::async(std::launch::async, [&pushFunc](){ pushFunc(300, 599); });
    std::async(std::launch::async, [&pushFunc](){ pushFunc(600, 999); });

    for (auto& f : futures)
    {
        f.wait();
    }

    // merge the result buckets into one vector
    std::vector<uint32_t> result;
    for (const auto& res : results)
    {
        result.insert(result.end(), res.begin(), res.end());
    }

    // sort to help with comparing against expected
    std::sort(result.begin(), result.end());

    REQUIRE(result == expected);
}

/*********************************************
 * HasValues
 ********************************************/

TEST_CASE("HasValues - no values - returns false", "[concurrency]")
{
    channel<uint32_t> c;

    auto result = c.HasValues();

    REQUIRE_FALSE(result);
}

TEST_CASE("HasValues - pushed values - returns true", "[concurrency]")
{
    channel<uint32_t> c;

    c.Push(1);
    c.Push(2);
    c.Push(3);

    auto result = c.HasValues();

    REQUIRE(result);
}

TEST_CASE("HasValues - after values removed - returns false", "[concurrency]")
{
    channel<uint32_t> c;

    c.Push(1);
    c.Push(2);
    c.Push(3);

    auto result = c.HasValues();

    REQUIRE(result);

    auto values = c.GetAll();

    result = c.HasValues();

    REQUIRE_FALSE(result);
}
