#include <thread>

#include "catch2/catch.hpp"
#include "markdown/markdown.h"

using namespace std::literals;
using namespace projectfarm::shared::markdown;
using namespace projectfarm::shared::graphics;

/*********************************************
 * GetTextParts
 ********************************************/

TEST_CASE("GetTextParts - empty text - returns empty result", "[markdown]")
{
    auto text = ""s;

    auto result = GetTextParts(text);

    REQUIRE(result.empty());
}

TEST_CASE("GetTextParts - no markdown - returns single text part", "[markdown]")
{
    auto text = "no markdown here"s;

    auto result = GetTextParts(text);

    TextPart expected
    {
        TextPartTypes::Normal,
        0,
        16,
        text,
    };

    REQUIRE(result.size() == 1);
    REQUIRE(result[0] == expected);
}

TEST_CASE("GetTextParts - markdown - returns correct text parts", "[markdown]")
{
    std::vector<std::string> markdown
    {
        "*italic text*",
        "a*italic text*b",
        "this *is* some *italic* text",
        "*incomplete italic text",
        "_bold text_",
        "a_bold text_b",
        "this _is_ some _bold_ text",
        "_incomplete bold text",
        "$bold italic text$",
        "a$bold italic text$b",
        "this $is$ some $bold italic$ text",
        "$incomplete bold italic text",
        "(red)red text",
        "(red)red (blue)text",
        "(red)red (blue)text (green",
        "(#12345678)hex color",
        "*(#12345678)hex color*",
        "default color (red)red text",
        "*\\*backslash*",
        "_\\_backslash_",
        "$\\$backslash$",
        "\\(backslash)",
        "asdfasdf_",
        "asdfasdf_asdf",
        "here is some _bold (green)text_ that I have",
    };

    std::vector<std::vector<TextPart>> expected
    {
        {
            { TextPartTypes::Italic, 1, 13, "italic text" }
        },
        {
            { TextPartTypes::Normal, 0, 1, "a" },
            { TextPartTypes::Italic, 2, 14, "italic text" },
            { TextPartTypes::Normal, 14, 15, "b" },
        },
        {
            { TextPartTypes::Normal, 0, 5, "this " },
            { TextPartTypes::Italic, 6, 9, "is" },
            { TextPartTypes::Normal, 9, 15, " some " },
            { TextPartTypes::Italic, 16, 23, "italic" },
            { TextPartTypes::Normal, 23, 28, " text" },
        },
        {
            { TextPartTypes::Italic, 0, 23, "*incomplete italic text" }
        },
        {
            { TextPartTypes::Bold, 1, 11, "bold text" }
        },
        {
            { TextPartTypes::Normal, 0, 1, "a" },
            { TextPartTypes::Bold, 2, 12, "bold text" },
            { TextPartTypes::Normal, 12, 13, "b" },
        },
        {
            { TextPartTypes::Normal, 0, 5, "this " },
            { TextPartTypes::Bold, 6, 9, "is" },
            { TextPartTypes::Normal, 9, 15, " some " },
            { TextPartTypes::Bold, 16, 21, "bold" },
            { TextPartTypes::Normal, 21, 26, " text" },
        },
        {
            { TextPartTypes::Bold, 0, 21, "_incomplete bold text" }
        },
        {
            { TextPartTypes::BoldItalic, 1, 18, "bold italic text" }
        },
        {
            { TextPartTypes::Normal, 0, 1, "a" },
            { TextPartTypes::BoldItalic, 2, 19, "bold italic text" },
            { TextPartTypes::Normal, 19, 20, "b" },
        },
        {
            { TextPartTypes::Normal, 0, 5, "this " },
            { TextPartTypes::BoldItalic, 6, 9, "is" },
            { TextPartTypes::Normal, 9, 15, " some " },
            { TextPartTypes::BoldItalic, 16, 28, "bold italic" },
            { TextPartTypes::Normal, 28, 33, " text" },
        },
        {
            { TextPartTypes::BoldItalic, 0, 28, "$incomplete bold italic text" }
        },
        {
            { TextPartTypes::Normal, 5, 13, "red text", colors::Red }
        },
        {
            { TextPartTypes::Normal, 5, 15, "red ", colors::Red },
            { TextPartTypes::Normal, 15, 19, "text", colors::Blue },
        },
        {
            { TextPartTypes::Normal, 5, 15, "red ", colors::Red },
            { TextPartTypes::Normal, 15, 26, "text (green", colors::Blue },
        },
        {
            { TextPartTypes::Normal, 11, 20, "hex color", *colors::FromHexString("#12345678") }
        },
        {
            { TextPartTypes::Italic, 12, 22, "hex color", *colors::FromHexString("#12345678") }
        },
        {
            { TextPartTypes::Normal, 0, 19, "default color " },
            { TextPartTypes::Normal, 19, 27, "red text", colors::Red },
        },
        {
            { TextPartTypes::Italic, 2, 13, "*backslash" }
        },
        {
            { TextPartTypes::Bold, 2, 13, "_backslash" }
        },
        {
            { TextPartTypes::BoldItalic, 2, 13, "$backslash" }
        },
        {
            { TextPartTypes::Normal, 0, 12, "(backslash)" }
        },
        {
            { TextPartTypes::Normal, 0, 8, "asdfasdf" },
            { TextPartTypes::Bold, 0, 9, "_" }
        },
        {
            { TextPartTypes::Normal, 0, 8, "asdfasdf" },
            { TextPartTypes::Bold, 0, 13, "_asdf" }
        },
        {
            { TextPartTypes::Normal, 0, 13, "here is some " },
            { TextPartTypes::Bold, 14, 26, "bold " },
            { TextPartTypes::Bold, 26, 31, "text", colors::Green },
            { TextPartTypes::Normal, 31, 43, " that I have", colors::Green },
        },
    };

    for (auto i = 0u; i < markdown.size(); ++i)
    {
        auto result = GetTextParts(markdown[i]);

        std::string info = markdown[i];
        for (const auto& res : result)
        {
            info += " : " + res._text;
        }
        INFO(info);
        REQUIRE(result.size() == expected[i].size());
        REQUIRE(result == expected[i]);
    }
}

TEST_CASE("GetMarkdownPositionFromPartPosition - empty text - returns 0", "[markdown]")
{
    auto text = ""s;
    auto position = 0u;

    auto result = GetMarkdownPositionFromPartPosition(text, position);

    REQUIRE(result == 0);
}

TEST_CASE("GetMarkdownPositionFromPartPosition - position is 0 - returns 0", "[markdown]")
{
    auto text = "text"s;
    auto position = 0u;

    auto result = GetMarkdownPositionFromPartPosition(text, position);

    REQUIRE(result == 0);
}

TEST_CASE("GetMarkdownPositionFromPartPosition - valid text - returns correct position", "[markdown]")
{
    // the text, the part position, the expected markdown position
    std::vector<std::tuple<std::string, uint32_t, uint32_t>> texts
    {
        { "text", 0, 0 },
        { "text", 1, 1 },
        { "text", 4, 4 },
        { "*text*", 4, 6 },
        { "text(blue)", 4, 10 },
        { "text(blue) ", 4, 10 },
        { "text(blue) ", 5, 11 },
        { "here is some _bold (green)text_ that I have", 0, 0 },
        { "here is some _bold (green)text_ that I have", 1, 1 },
        { "here is some _bold (green)text_ that I have", 15, 16 },
        { "here is some _bold (green)text_ that I have", 18, 26 },
        { "here is some _bold (green)text_ that I have", 29, 38 },
        { "here is some _bold (green)text_ that I have", 34, 43 },
        { "(blue)", 0, 6 },
        { "(blue)hi", 0, 6 },
        { "(blue)hi", 1, 7 },
        { "(blue)hi", 2, 8 },
        { "$text$(blue)", 4, 12 },
        { "$text$(blue)hi", 4, 12 },
        { "$text$(blue)hi", 5, 13 },
    };

    for (auto &[text, pos, expected] : texts)
    {
        auto result = GetMarkdownPositionFromPartPosition(text, pos);

        REQUIRE(result == expected);
    }
}
