#include <vector>
#include <cstdint>

#include "catch2/catch.hpp"
#include "math/lerper.h"

TEST_CASE("Positive low start to positive high end", "[lerper]")
{
    projectfarm::shared::math::Lerper<int32_t> lerper;

    auto start = 0u;
    auto end = 10u;
    auto time = 10u;
    lerper.Set(start, end, time);

    auto tick = 1u;
    for (auto i = 0u; i < time; ++i)
    {
        auto [result, finished] = lerper.Get(tick);

        if (i < time - 1)
        {
            REQUIRE_FALSE(finished);
        }
        else
        {
            REQUIRE(finished);
        }

        REQUIRE(static_cast<uint32_t>(result) == i + 1);
    }
}

TEST_CASE("Positive high start to positive low end", "[lerper]")
{
    projectfarm::shared::math::Lerper<int32_t> lerper;

    auto start = 10u;
    auto end = 0u;
    auto time = 10u;
    lerper.Set(start, end, time);

    auto tick = 1u;
    for (auto i = 0u; i < time; ++i)
    {
        auto [result, finished] = lerper.Get(tick);

        if (i < time - 1)
        {
            REQUIRE_FALSE(finished);
        }
        else
        {
            REQUIRE(finished);
        }

        REQUIRE(static_cast<uint32_t>(result) == time - i - 1);
    }
}

TEST_CASE("Negative low start to positive high end", "[lerper]")
{
    projectfarm::shared::math::Lerper<int32_t> lerper;

    auto start = -10;
    auto end = 10;
    auto time = 10u;
    lerper.Set(start, end, time);

    std::vector<int32_t> expected {-8, -6, -4, -2, 0, 2, 4, 6, 8, 10};

    auto tick = 1u;
    for (auto i = 0u; i < time; ++i)
    {
        auto [result, finished] = lerper.Get(tick);

        if (i < time - 1)
        {
            REQUIRE_FALSE(finished);
        }
        else
        {
            REQUIRE(finished);
        }

        REQUIRE(result == expected[i]);
    }
}

TEST_CASE("Positive high start to negative low end", "[lerper]")
{
    projectfarm::shared::math::Lerper<int32_t> lerper;

    auto start = 10;
    auto end = -10;
    auto time = 10u;
    lerper.Set(start, end, time);

    std::vector<int32_t> expected {8, 6, 4, 2, 0, -2, -4, -6, -8, -10};

    auto tick = 1u;
    for (auto i = 0u; i < time; ++i)
    {
        auto [result, finished] = lerper.Get(tick);

        if (i < time - 1)
        {
            REQUIRE_FALSE(finished);
        }
        else
        {
            REQUIRE(finished);
        }

        REQUIRE(result == expected[i]);
    }
}

TEST_CASE("Negative low start to negative high end", "[lerper]")
{
    projectfarm::shared::math::Lerper<int32_t> lerper;

    auto start = -20;
    auto end = -10;
    auto time = 10u;
    lerper.Set(start, end, time);

    std::vector<int32_t> expected {-19, -18, -17, -16, -15, -14, -13, -12, -11, -10};

    auto tick = 1u;
    for (auto i = 0u; i < time; ++i)
    {
        auto [result, finished] = lerper.Get(tick);

        if (i < time - 1)
        {
            REQUIRE_FALSE(finished);
        }
        else
        {
            REQUIRE(finished);
        }

        REQUIRE(result == expected[i]);
    }
}

TEST_CASE("Negative high start to negative low end", "[lerper]")
{
    projectfarm::shared::math::Lerper<int32_t> lerper;

    auto start = -10;
    auto end = -20;
    auto time = 10u;
    lerper.Set(start, end, time);

    std::vector<int32_t> expected {-11, -12, -13, -14, -15, -16, -17, -18, -19, -20};

    auto tick = 1u;
    for (auto i = 0u; i < time; ++i)
    {
        auto [result, finished] = lerper.Get(tick);

        if (i < time - 1)
        {
            REQUIRE_FALSE(finished);
        }
        else
        {
            REQUIRE(finished);
        }

        REQUIRE(result == expected[i]);
    }
}

TEST_CASE("Start and end are the same", "[lerper]")
{
    projectfarm::shared::math::Lerper<int32_t> lerper;

    auto start = 5u;
    auto end = 5u;
    auto time = 10u;
    lerper.Set(start, end, time);

    std::vector<int32_t> expected {5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

    auto tick = 1u;
    for (auto i = 0u; i < time; ++i)
    {
        auto [result, finished] = lerper.Get(tick);

        if (i < time - 1)
        {
            REQUIRE_FALSE(finished);
        }
        else
        {
            REQUIRE(finished);
        }

        REQUIRE(result == expected[i]);
    }
}

TEST_CASE("High start to low end values clamp", "[lerper]")
{
    projectfarm::shared::math::Lerper<int32_t> lerper;

    auto start = 10;
    auto end = -10;
    auto time = 10u;
    lerper.Set(start, end, time);

    std::vector<int32_t> expected {8, 6, 4, 2, 0, -2, -4, -6, -8, -10, -10, -10, -10, -10, -10, -10};

    auto tick = 1u;
    for (auto i = 0u; i < time + 5; ++i)
    {
        auto [result, finished] = lerper.Get(tick);

        if (i < time - 1)
        {
            REQUIRE_FALSE(finished);
        }
        else
        {
            REQUIRE(finished);
        }

        REQUIRE(result == expected[i]);
    }
}

TEST_CASE("Low start to high end values clamp", "[lerper]")
{
    projectfarm::shared::math::Lerper<int32_t> lerper;

    auto start = -10;
    auto end = 10;
    auto time = 10u;
    lerper.Set(start, end, time);

    std::vector<int32_t> expected {-8, -6, -4, -2, 0, 2, 4, 6, 8, 10, 10, 10, 10, 10, 10};

    auto tick = 1u;
    for (auto i = 0u; i < time + 5; ++i)
    {
        auto [result, finished] = lerper.Get(tick);

        if (i < time - 1)
        {
            REQUIRE_FALSE(finished);
        }
        else
        {
            REQUIRE(finished);
        }

        REQUIRE(result == expected[i]);
    }
}