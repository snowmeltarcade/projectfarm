#include "catch2/catch.hpp"
#include "css/css.h"

using namespace std::literals;
using namespace projectfarm::shared::css;

/*********************************************
 * LoadFromRaw
 ********************************************/

TEST_CASE("LoadFromRaw - empty string - returns false", "[css]")
{
    auto emptyString = "";

    auto result = LoadFromRaw(emptyString);

    REQUIRE_FALSE(result);
}
