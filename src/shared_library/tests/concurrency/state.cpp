#include <future>
#include <vector>

#include "catch2/catch.hpp"
#include "concurrency/state.h"

using namespace std::literals;
using namespace projectfarm::shared::concurrency;

/*********************************************
 * SetBool
 ********************************************/

TEST_CASE("SetBool - set value on single thread - sets value", "[concurrency]")
{
    auto expected = true;
    auto key = "key"s;

    state::SetBool(key, expected);

    auto result = state::GetBool(key);

    REQUIRE(result == expected);
}

TEST_CASE("SetBool - set value on multiple threads - sets value", "[concurrency]")
{
    auto expected = true;
    auto key = "key"s;

    std::vector<std::future<void>> futures;

    auto threadCount = 5;
    for (auto i {0}; i < threadCount; ++i)
    {
        auto future = std::async(std::launch::async, [key, expected](){ state::SetBool(key, expected); });
        futures.emplace_back(std::move(future));
    }

    for (auto& future : futures)
    {
        future.wait();
    }

    auto result = state::GetBool(key);

    REQUIRE(result == expected);
}

/*********************************************
 * GetBool
 ********************************************/

TEST_CASE("GetBool - get value on single thread - gets value", "[concurrency]")
{
    auto expected = true;
    auto key = "key"s;

    state::SetBool(key, expected);

    auto result = state::GetBool(key);

    REQUIRE(result == expected);
}

TEST_CASE("GetBool - gets value on multiple threads - gets value", "[concurrency]")
{
    auto expected = true;
    auto key = "key"s;

    std::vector<std::future<void>> futures;

    state::SetBool(key, expected);

    auto threadCount = 5;
    for (auto i {0}; i < threadCount; ++i)
    {
        auto future = std::async(std::launch::async,
                                 [key, expected]()
                                 {
                                     auto result = state::GetBool(key);

                                     REQUIRE(result == expected);
                                 });

        futures.emplace_back(std::move(future));
    }

    for (auto& future : futures)
    {
        future.wait();
    }
}

TEST_CASE("GetBool - set value to quit loops on multiple threads - quits loops", "[concurrency]")
{
    auto key = "key"s;

    std::vector<std::future<void>> futures;

    auto runner = [key](){ while (state::GetBool(key)){} };

    state::SetBool(key, true);

    auto threadCount = 5;
    for (auto i {0}; i < threadCount; ++i)
    {
        auto future = std::async(std::launch::async, runner);

        futures.emplace_back(std::move(future));
    }

    state::SetBool(key, false);

    for (auto& future : futures)
    {
        future.wait();
    }
}
