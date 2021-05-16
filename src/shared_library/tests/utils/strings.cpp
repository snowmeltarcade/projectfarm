#include <string>
#include <array>

#include "catch2/catch.hpp"
#include "utils/strings.h"

using namespace std::literals;
using namespace projectfarm::shared::utils;

/*********************************************
 * trim
 ********************************************/

TEST_CASE("trim - empty string - returns empty string", "[strings]")
{
    auto s = "";
    auto res = trim(s);

    REQUIRE(res.empty());
}

TEST_CASE("trim - whitespace string - returns empty string", "[strings]")
{
    auto s = "     ";
    auto res = trim(s);

    REQUIRE(res.empty());
}

TEST_CASE("trim - string with whitespace at beginning - returns trimmed string", "[strings]")
{
    auto s = "  my text";
    auto res = trim(s);

    auto expected = "my text";
    REQUIRE(res == expected);
}

TEST_CASE("trim - string with whitespace at end - returns trimmed string", "[strings]")
{
    auto s = "my text   ";
    auto res = trim(s);

    auto expected = "my text";
    REQUIRE(res == expected);
}

TEST_CASE("trim - string with whitespace at beginning and end - returns trimmed string", "[strings]")
{
    auto s = "    my text   ";
    auto res = trim(s);

    auto expected = "my text";
    REQUIRE(res == expected);
}

/*********************************************
 * ltrim
 ********************************************/

TEST_CASE("ltrim - empty string - returns empty string", "[strings]")
{
    auto s = "";
    auto res = ltrim(s);

    REQUIRE(res.empty());
}

TEST_CASE("ltrim - whitespace string - returns empty string", "[strings]")
{
    auto s = "     ";
    auto res = ltrim(s);

    REQUIRE(res.empty());
}

TEST_CASE("ltrim - string with whitespace at beginning and end - returns left trimmed string", "[strings]")
{
    auto s = "    my text   ";
    auto res = ltrim(s);

    auto expected = "my text   ";
    REQUIRE(res == expected);
}

/*********************************************
 * rtrim
 ********************************************/

TEST_CASE("rtrim - empty string - returns empty string", "[strings]")
{
    auto s = "";
    auto res = rtrim(s);

    REQUIRE(res.empty());
}

TEST_CASE("rtrim - whitespace string - returns empty string", "[strings]")
{
    auto s = "     ";
    auto res = rtrim(s);

    REQUIRE(res.empty());
}

TEST_CASE("rtrim - string with whitespace at beginning and end - returns right trimmed string", "[strings]")
{
    auto s = "    my text   ";
    auto res = rtrim(s);

    auto expected = "    my text";
    REQUIRE(res == expected);
}

/*********************************************
 * tolower
 ********************************************/

TEST_CASE("tolower - empty string - returns empty string", "[strings]")
{
    auto s = "";
    auto res = tolower(s);

    REQUIRE(res.empty());
}

TEST_CASE("tolower - string with uppercase letters - returns lowercase string", "[strings]")
{
    auto s = " My StRiNG !! ";
    auto res = tolower(s);

    auto expected = " my string !! ";
    REQUIRE(res == expected);
}

/*********************************************
 * toupper
 ********************************************/

TEST_CASE("toupper - empty string - returns empty string", "[strings]")
{
    auto s = "";
    auto res = toupper(s);

    REQUIRE(res.empty());
}

TEST_CASE("toupper - string with lowercase letters - returns uppercase string", "[strings]")
{
    auto s = " My StRiNG !! ";
    auto res = toupper(s);

    auto expected = " MY STRING !! ";
    REQUIRE(res == expected);
}

/*********************************************
 * toarray
 ********************************************/

TEST_CASE("toarray - valid string - returns array", "[strings]")
{
    constexpr auto s = "my string";
    auto res = toarray<9>(s);

    auto expected = std::array<uint8_t, 9>(
    {
        'm',
        'y',
        ' ',
        's',
        't',
        'r',
        'i',
        'n',
        'g',
    });

    REQUIRE(res == expected);
}

TEST_CASE("toarray - string to short - returns array with backslash 0 as remaining characters", "[strings]")
{
    constexpr auto s = "my str";
    auto res = toarray<9>(s);

    auto expected = std::array<uint8_t, 9>(
    {
        'm',
        'y',
        ' ',
        's',
        't',
        'r',
        '\0',
        '\0',
        '\0',
    });

    REQUIRE(res == expected);
}

TEST_CASE("toarray - string to long - returns array of the specified length", "[strings]")
{
    constexpr auto s = "my string my string my string";
    auto res = toarray<11>(s);

    auto expected = std::array<uint8_t, 11>(
    {
        'm',
        'y',
        ' ',
        's',
        't',
        'r',
        'i',
        'n',
        'g',
        ' ',
        'm',
    });

    REQUIRE(res == expected);
}

/*********************************************
 * fromarray
 ********************************************/

TEST_CASE("fromarray - valid array - returns string", "[strings]")
{
    constexpr auto a = std::array<uint8_t, 9>(
    {
    'm',
    'y',
    ' ',
    's',
    't',
    'r',
    'i',
    'n',
    'g',
    });

    auto res = fromarray<9>(a);

    constexpr auto expected = "my string";

    REQUIRE(res == expected);
}

/*********************************************
 * split
 ********************************************/

TEST_CASE("split - empty divisor - returns empty list", "[strings]")
{
    constexpr auto divisor = "";
    constexpr auto s = "mykey=myvalue";

    auto res = split(divisor, s);

    std::vector<std::string> expected;

    REQUIRE(res == expected);
}

TEST_CASE("split - empty string to split - returns empty list", "[strings]")
{
    constexpr auto divisor = "=";
    constexpr auto s = "";

    auto res = split(divisor, s);

    std::vector<std::string> expected;

    REQUIRE(res == expected);
}

TEST_CASE("split - single character - returns text", "[strings]")
{
    constexpr auto divisor = "=";
    constexpr auto s = "a";

    auto res = split(divisor, s);

    std::vector<std::string> expected
    {
        s,
    };

    REQUIRE(res == expected);
}

TEST_CASE("split - no divisor - returns text", "[strings]")
{
    constexpr auto divisor = "=";
    constexpr auto s = "mykey&myvalue";

    auto res = split(divisor, s);

    std::vector<std::string> expected
    {
        s,
    };

    REQUIRE(res == expected);
}

TEST_CASE("split - two parts - returns results list", "[strings]")
{
    constexpr auto divisor = "=";
    constexpr auto s = "mykey=myvalue";

    auto res = split(divisor, s);

    std::vector<std::string> expected
    {
        "mykey",
        "myvalue",
    };

    REQUIRE(res == expected);
}

TEST_CASE("split - 5 parts - returns results list", "[strings]")
{
    constexpr auto divisor = "=";
    constexpr auto s = "mykey=myvalue=1=234=56789";

    auto res = split(divisor, s);

    std::vector<std::string> expected
    {
        "mykey",
        "myvalue",
        "1",
        "234",
        "56789",
    };

    REQUIRE(res == expected);
}

TEST_CASE("split - multi-character delimiter - returns results list", "[strings]")
{
    constexpr auto divisor = "<>";
    constexpr auto s = "mykey<>myvalue<>1<>234<>56789";

    auto res = split(divisor, s);

    std::vector<std::string> expected
    {
        "mykey",
        "myvalue",
        "1",
        "234",
        "56789",
    };

    REQUIRE(res == expected);
}

/*********************************************
 * startsWith
 ********************************************/

TEST_CASE("startsWith - empty string to check - returns false", "[strings]")
{
    constexpr auto stringToCheck = "";
    constexpr auto starts = "start part";

    auto res = startsWith(stringToCheck, starts);

    REQUIRE_FALSE(res);
}

TEST_CASE("startsWith - empty value to check for - returns false", "[strings]")
{
    constexpr auto stringToCheck = "start part of this string";
    constexpr auto starts = "";

    auto res = startsWith(stringToCheck, starts);

    REQUIRE_FALSE(res);
}

TEST_CASE("startsWith - value to check for longer than string to check - returns false", "[strings]")
{
    constexpr auto stringToCheck = "string";
    constexpr auto starts = "long value to check for";

    auto res = startsWith(stringToCheck, starts);

    REQUIRE_FALSE(res);
}

TEST_CASE("startsWith - string starts with value to check for - returns true", "[strings]")
{
    constexpr auto stringToCheck = "start part of this string";
    constexpr auto starts = "start part";

    auto res = startsWith(stringToCheck, starts);

    REQUIRE(res);
}

TEST_CASE("startsWith - string does not start with value to check for - returns true", "[strings]")
{
    constexpr auto stringToCheck = "start part of this string";
    constexpr auto starts = "abcd";

    auto res = startsWith(stringToCheck, starts);

    REQUIRE_FALSE(res);
}

/*********************************************
 * endsWith
 ********************************************/

TEST_CASE("endsWith - empty string to check - returns false", "[strings]")
{
    constexpr auto stringToCheck = "";
    constexpr auto ends = "end part";

    auto res = endsWith(stringToCheck, ends);

    REQUIRE_FALSE(res);
}

TEST_CASE("endsWith - empty value to check for - returns false", "[strings]")
{
    constexpr auto stringToCheck = "start part of this string";
    constexpr auto ends = "";

    auto res = endsWith(stringToCheck, ends);

    REQUIRE_FALSE(res);
}

TEST_CASE("endsWith - value to check for longer than string to check - returns false", "[strings]")
{
    constexpr auto stringToCheck = "string";
    constexpr auto ends = "long value to check for";

    auto res = endsWith(stringToCheck, ends);

    REQUIRE_FALSE(res);
}

TEST_CASE("endsWith - string ends with value to check for - returns true", "[strings]")
{
    constexpr auto stringToCheck = "this string has an end part";
    constexpr auto ends = "end part";

    auto res = endsWith(stringToCheck, ends);

    REQUIRE(res);
}

TEST_CASE("endsWith - string does not end with value to check for - returns true", "[strings]")
{
    constexpr auto stringToCheck = "this string has an end part";
    constexpr auto ends = "abcd";

    auto res = endsWith(stringToCheck, ends);

    REQUIRE_FALSE(res);
}