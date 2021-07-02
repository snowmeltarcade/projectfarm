#include "catch2/catch.hpp"
#include "graphics/colors/color.h"

using namespace std::literals;
using namespace projectfarm::shared::graphics::colors;

/*********************************************
 * operator ==
 ********************************************/

TEST_CASE("operator == - colors equal - returns true", "[graphics/colors]")
{
    Color c1 { 1, 2, 3, 4 };
    Color c2 { 1, 2, 3, 4 };

    REQUIRE(c1 == c2);
}

TEST_CASE("operator == - colors not equal - returns false", "[graphics/colors]")
{
    Color c1 { 1, 2, 3, 4 };
    Color c2 { 2, 3, 4, 5 };

    REQUIRE_FALSE(c1 == c2);
}

/*********************************************
 * operator !=
 ********************************************/

TEST_CASE("operator != - colors equal - returns false", "[graphics/colors]")
{
    Color c1 { 1, 2, 3, 4 };
    Color c2 { 1, 2, 3, 4 };

    REQUIRE_FALSE(c1 != c2);
}

TEST_CASE("operator != - colors not equal - returns true", "[graphics/colors]")
{
    Color c1 { 1, 2, 3, 4 };
    Color c2 { 2, 3, 4, 5 };

    REQUIRE(c1 != c2);
}

/*********************************************
 * FromHexString
 ********************************************/

TEST_CASE("FromHexString - empty string - returns empty", "[graphics/colors]")
{
    auto s = ""s;

    auto res = FromHexString(s);

    REQUIRE_FALSE(res.has_value());
}

TEST_CASE("FromHexString - invalid hex string - returns empty", "[graphics/colors]")
{
    std::vector<std::string> invalid
    {
        "123456",
        "#qwert",
    };

    for (const auto& s : invalid)
    {
        auto res = FromHexString(s);

        REQUIRE_FALSE(res.has_value());
    }
}

TEST_CASE("FromHexString - valid hex strings - returns correct color", "[graphics/colors]")
{
    std::vector<std::pair<std::string, Color>> values
    {
        { "#FFFFFFFF", { 255, 255, 255, 255 } },
        { "#FF000000", { 255, 0, 0, 0 } },
        { "#00FF0000", { 0, 255, 0, 0 } },
        { "#0000FF00", { 0, 0, 255, 0 } },
        { "#000000FF", { 0, 0, 0, 255 } },
        { "#00000000", { 0, 0, 0, 0 } },
        { "#12345678", { 18, 52, 86, 120 } },
        { "#78563412", { 120, 86, 52, 18 } },
        { "#FFFFFF", { 255, 255, 255, 255 } },
        { "#FF0000", { 255, 0, 0, 255 } },
    };

    for (const auto& [hex, expected] : values)
    {
        auto res = FromHexString(hex);

        INFO(hex);
        REQUIRE(res.has_value());
        REQUIRE(res->r == expected.r);
        REQUIRE(res->g == expected.g);
        REQUIRE(res->b == expected.b);
        REQUIRE(res->a == expected.a);
    }
}

/*********************************************
 * FromString
 ********************************************/

TEST_CASE("FromString - empty string - returns empty", "[graphics/colors]")
{
    auto s = ""s;

    auto res = FromString(s);

    REQUIRE_FALSE(res.has_value());
}

TEST_CASE("FromString - invalid color name - returns empty", "[graphics/colors]")
{
    auto s = "invalid color name"s;

    auto res = FromString(s);

    REQUIRE_FALSE(res.has_value());
}

TEST_CASE("FromString - invalid hex string - returns empty", "[graphics/colors]")
{
    auto s = "#qwert"s;

    auto res = FromString(s);

    REQUIRE_FALSE(res.has_value());
}

TEST_CASE("FromString - valid hex strings - returns correct color", "[graphics/colors]")
{
    std::vector<std::pair<std::string, Color>> values
    {
        { "#FFFFFFFF", { 255, 255, 255, 255 } },
        { "#FF000000", { 255, 0, 0, 0 } },
        { "#00FF0000", { 0, 255, 0, 0 } },
        { "#0000FF00", { 0, 0, 255, 0 } },
        { "#000000FF", { 0, 0, 0, 255 } },
        { "#00000000", { 0, 0, 0, 0 } },
        { "#12345678", { 18, 52, 86, 120 } },
        { "#78563412", { 120, 86, 52, 18 } },
        { "#FFFFFF", { 255, 255, 255, 255 } },
        { "#FF0000", { 255, 0, 0, 255 } },
    };

    for (const auto& [hex, expected] : values)
    {
        auto res = FromString(hex);

        REQUIRE(res.has_value());
        REQUIRE(res->r == expected.r);
        REQUIRE(res->g == expected.g);
        REQUIRE(res->b == expected.b);
        REQUIRE(res->a == expected.a);
    }
}

TEST_CASE("FromString - valid color names - returns color", "[graphics/colors]")
{
    for (const auto& [name, expected] : ColorNames)
    {
        auto res = FromString(name);

        REQUIRE(res.has_value());
        REQUIRE(res == expected);
    }
}

/*********************************************
 * ToInt
 ********************************************/

TEST_CASE("ToInt - returns valid int", "[graphics/colors]")
{
    // color expected
    std::vector<std::pair<Color, uint32_t>> colors
    {
        { {0, 0, 0, 0}, 0 },
        { {170, 170, 170, 170}, 2863311530 },
        { {34, 85, 170, 255}, 576039679 },
        { {255, 255, 255, 255}, 4294967295 },
    };

    for (const auto& [color, expected] : colors)
    {
        auto res = color.ToInt();

        REQUIRE(res == expected);
    }
}

/*********************************************
 * ToHexString
 ********************************************/

TEST_CASE("ToHexString - returns correct hex string", "[graphics/colors]")
{
    std::vector<std::pair<Color, std::string>> values
    {
        { { 255, 255, 255, 255 }, "#FFFFFFFF" },
        { { 255, 0, 0, 0 }, "#FF000000" },
        { { 0, 255, 0, 0 }, "#00FF0000" },
        { { 0, 0, 255, 0 }, "#0000FF00" },
        { { 0, 0, 0, 255 }, "#000000FF" },
        { { 0, 0, 0, 0 }, "#00000000" },
        { { 18, 52, 86, 120 }, "#12345678" },
        { { 120, 86, 52, 18 }, "#78563412" },
        { { 255, 255, 255, 255 }, "#FFFFFFFF" },
        { { 255, 0, 0, 255 }, "#FF0000FF" },
    };

    for (const auto& [color, expected] : values)
    {
        auto res = color.ToHexString();

        INFO(color.ToString());
        REQUIRE(res == expected);
    }
}

/*********************************************
 * FromInt
 ********************************************/

TEST_CASE("FromInt - returns valid colors", "[graphics/colors]")
{
    // color expected
    std::vector<std::pair<uint32_t, Color>> colors
    {
        { 0, {0, 0, 0, 0} },
        { 2863311530, {170, 170, 170, 170} },
        { 576039679, {34, 85, 170, 255} },
        { 4294967295, {255, 255, 255, 255} },
    };

    for (const auto& [i, expected] : colors)
    {
        Color res;
        res.FromInt(i);

        INFO("Res: " + res.ToString() + " Expected: " + expected.ToString());
        REQUIRE(res == expected);
    }
}
