#include <future>
#include <vector>

#include "catch2/catch.hpp"
#include "concurrency/channel.h"

using namespace std::literals;
using namespace projectfarm::shared::concurrency;

/*********************************************
 * Push
 ********************************************/

TEST_CASE("Push - set value on single thread - sets value", "[concurrency]")
{
    auto expected = true;
    auto key = "key"s;

    channel::Push(key, expected);

    auto result = channel::Get(key);

    REQUIRE(result.size() == 1);
    REQUIRE(result[0] == expected);
}
