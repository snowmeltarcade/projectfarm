#include <cstdint>

#include "catch2/catch.hpp"
#include "math/hex.h"

using namespace std::literals;
using namespace projectfarm::shared::math;

/*********************************************
 * HexToDec - char
 ********************************************/

TEST_CASE("HexToDec char - Invalid hex - returns empty", "[math]")
{
    auto validHex = "0123456789abcdefABCDEF"s;

    for (auto c = 0u; c <= 255u; ++c)
    {
        // only check invalid hex
        if (validHex.find(static_cast<char>(c)) == std::string::npos)
        {
            auto res = HexToDec<uint8_t>(static_cast<char>(c));
            REQUIRE_FALSE(res.has_value());
        }
    }
}

TEST_CASE("HexToDec char - Valid hex - returns correct decimal value", "[math]")
{
    auto validHex = "0123456789abcdefABCDEF"s;

    std::vector<uint32_t> expected
    {
        // match the validHex above
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 10, 11, 12, 13, 14, 15,
    };

    for (auto i = 0u; i < validHex.length(); ++i)
    {
        auto res = HexToDec<uint8_t>(validHex[i]);

        REQUIRE(res.has_value());
        REQUIRE(*res == expected[i]);
    }
}

/*********************************************
 * HexToDec - std::string_view
 ********************************************/

TEST_CASE("HexToDec std::string_view - Empty string - returns empty", "[math]")
{
    auto invalid = "";

    auto res = HexToDec<uint32_t>(invalid);

    REQUIRE_FALSE(res.has_value());
}

TEST_CASE("HexToDec std::string_view - Invalid hex - returns empty", "[math]")
{
    auto invalid = "my invalid hex";

    auto res = HexToDec<uint32_t>(invalid);

    REQUIRE_FALSE(res.has_value());
}

TEST_CASE("HexToDec std::string_view - Valid hex - returns correct value", "[math]")
{
    std::vector<std::pair<std::string, uint32_t>> values
    {
        { "0", 0 },
        { "F", 15 },
        { "00", 0 },
        { "FF", 255 },
        { "DEADBEEF", 3'735'928'559 },
        { "ABCD", 43981 },
        { "FFFFFFFF", 4'294'967'295 },
    };

    for (const auto& [hex, expected] : values)
    {
        auto res = HexToDec<uint32_t>(hex);

        REQUIRE(res.has_value());
        REQUIRE(*res == expected);
    }
}

/*********************************************
 * DecToHex - uint8_t
 ********************************************/

TEST_CASE("DecToHex uint8_t - valid input - returns valid hex", "[math]")
{
    std::vector<std::pair<uint8_t, std::string>> values
    {
        { 0u,   "00" },
        { 15u,  "0F" },
        { 100u, "64" },
        { 255u, "FF" },
    };

    for (const auto& [dec, expected] : values)
    {
        auto result = DecToHex(dec);

        REQUIRE(result == expected);
    }
}
