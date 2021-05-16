#include <thread>

#include "catch2/catch.hpp"
#include "time/stopwatch.h"

using namespace std::literals;
using namespace projectfarm::shared::time;

/*********************************************
 * Tick
 ********************************************/

TEST_CASE("Tick - 500ms clock - returns true on tick", "[time]")
{
    Stopwatch sw;

    sw.SetTargetMilliseconds(500);
    sw.Start();

    auto targetRunTime {5000};
    auto currentTime {0};

    auto result = 0u;

    while (true)
    {
        if (sw.Tick())
        {
            ++result;
        }

        auto frameTime = 50u;
        std::this_thread::sleep_for(std::chrono::milliseconds(frameTime));
        currentTime += frameTime;

        if (currentTime > targetRunTime)
        {
            break;
        }
    }

    auto expected = targetRunTime / sw.GetTargetMilliseconds();

    REQUIRE(result == expected);
}

TEST_CASE("Tick - 500ms clock - calls on tick callback", "[time]")
{
    Stopwatch sw;

    sw.SetTargetMilliseconds(500);
    sw.Start();

    auto targetRunTime {5000};
    auto currentTime {0};

    auto result = 0u;

    sw.SetOnTick([&result](){ ++result; });

    while (true)
    {
        sw.Tick();

        auto frameTime = 50u;
        std::this_thread::sleep_for(std::chrono::milliseconds(frameTime));
        currentTime += frameTime;

        if (currentTime > targetRunTime)
        {
            break;
        }
    }

    auto expected = targetRunTime / sw.GetTargetMilliseconds();

    REQUIRE(result == expected);
}