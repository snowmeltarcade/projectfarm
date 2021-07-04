#include <vector>
#include <string>
#include <tuple>

#include "catch2/catch.hpp"
#include "test_util.h"
#include "css/css.h"

using namespace std::literals;
using namespace projectfarm::shared::css;

/*********************************************
 * LoadFromRaw
 ********************************************/

TEST_CASE("LoadFromRaw - empty string - returns error", "[css]")
{
    auto emptyString = "";

    auto result = LoadFromRaw(emptyString);

    REQUIRE_FALSE(result);
}

TEST_CASE("LoadFromRaw - invalid CSS - returns error", "[css]")
{
    std::vector<std::string> invalidCSS
    {
        // no selector
        "{}",

        // no block
        "selector",

        // block not completed
        "selector {",

        // block not started
        "selector }",

        // block not started
        "selector }",
    };

    for (const auto& css : invalidCSS)
    {
        auto result = LoadFromRaw(css);

        INFO(css);
        REQUIRE_FALSE(result);
    }
}

TEST_CASE("LoadFromRaw - valid CSS - returns valid result", "[css]")
{
    std::vector<std::pair<std::string, CSSDocument>> validCSS;

    CSSDocument cssDocument;
    CSSClass cssClass;

    ////
    // empty block, single selector
    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector{}", cssDocument } );

    ////
    // empty block, single selector with UI event name
    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector@event-name" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector@event-name{}", cssDocument } );

    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector-1" } );
    cssDocument.Classes.push_back(cssClass);

    cssClass = CSSClass();
    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector-2" } );
    cssDocument.Classes.push_back(cssClass);

    cssClass = CSSClass();
    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector-3" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector-1{}selector-2{}selector-3{}", cssDocument } );

    ////
    // empty block, multiple selectors
    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector-1" } );
    cssClass.Selectors.push_back( { CSSSelectorTypes::Class, "selector-2" } );
    cssClass.Selectors.push_back( { CSSSelectorTypes::Id, "selector-3" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector-1,.selector-2,#selector-3{}", cssDocument } );

    ////
    // single attribute
    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector" } );
    cssClass.Attributes.push_back( { "key", "value" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector{key:value;}", cssDocument } );

    ////
    // multiple attributes
    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector" } );
    cssClass.Attributes.push_back( { "key1", "value1" } );
    cssClass.Attributes.push_back( { "key2", "value2" } );
    cssClass.Attributes.push_back( { "key3", "value3" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector{key1:value1;key2:value2;key3:value3;}", cssDocument } );

    ////
    // multiple attributes, multiple classes
    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector1" } );
    cssClass.Attributes.push_back( { "key1", "value1" } );
    cssClass.Attributes.push_back( { "key2", "value2" } );
    cssClass.Attributes.push_back( { "key3", "value3" } );
    cssDocument.Classes.push_back(cssClass);

    cssClass = CSSClass();
    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector2" } );
    cssClass.Attributes.push_back( { "key1", "value1" } );
    cssClass.Attributes.push_back( { "key2", "value2" } );
    cssDocument.Classes.push_back(cssClass);

    cssClass = CSSClass();
    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector3" } );
    cssClass.Selectors.push_back( { CSSSelectorTypes::Id, "id1" } );
    cssClass.Attributes.push_back( { "key1", "value1" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector1{key1:value1;key2:value2;key3:value3}selector2{key1:value1;key2:value2;}selector3,#id1{key1:value1;}", cssDocument } );

    ////
    // attributes with empty name or value are ignored
    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector1" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector1{:}", cssDocument } );

    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector1" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector1{:;}", cssDocument } );

    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector1" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector1{key:;}", cssDocument } );

    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector1" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector1{:value;}", cssDocument } );

    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector1" } );
    cssClass.Attributes.push_back( { "key1", "value1" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector1{:;key1:value1}", cssDocument } );

    ////
    // attributes with data location parameters
    ////
    cssDocument = CSSDocument();
    cssClass = CSSClass();

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector" } );
    cssClass.Attributes.push_back( { "key1", "{location1}/value1" } );
    cssClass.Attributes.push_back( { "key2", "{location2}/value2" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "selector{key1:{location1}/value1;key2:{location2}/value2}", cssDocument } );

    for (const auto& [css, doc] : validCSS)
    {
        auto result = LoadFromRaw(css);
        REQUIRE(result);

        INFO(css);
        REQUIRE(*result == doc);
    }
}

/*********************************************
 * LoadFromFile
 ********************************************/

TEST_CASE("LoadFromFile - file not exists - returns error", "[css]")
{
    std::filesystem::path path = "file does not exist.css";

    auto result = LoadFromFile(path);

    REQUIRE_FALSE(result);
}

TEST_CASE("LoadFromFile - invalid CSS - returns error", "[css]")
{
    std::filesystem::path path = GetCSSFilePath("invalid.css");

    auto result = LoadFromFile(path);

    REQUIRE_FALSE(result);
}

TEST_CASE("LoadFromFile - valid CSS - returns valid result", "[css]")
{
    std::vector<std::pair<std::string, CSSDocument>> validCSS;

    CSSDocument cssDocument;
    CSSClass cssClass;

    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector1" } );
    cssClass.Attributes.push_back( { "key1", "value1" } );
    cssClass.Attributes.push_back( { "key2", "value2" } );
    cssClass.Attributes.push_back( { "key3", "value3" } );
    cssDocument.Classes.push_back(cssClass);

    cssClass = CSSClass();
    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector2" } );
    cssClass.Attributes.push_back( { "key1", "value1" } );
    cssClass.Attributes.push_back( { "key2", "value2" } );
    cssDocument.Classes.push_back(cssClass);

    cssClass = CSSClass();
    cssClass.Selectors.push_back( { CSSSelectorTypes::Type, "selector3" } );
    cssClass.Selectors.push_back( { CSSSelectorTypes::Id, "id1" } );
    cssClass.Attributes.push_back( { "key1", "value1" } );
    cssDocument.Classes.push_back(cssClass);

    validCSS.push_back( { "valid.css", cssDocument } );

    for (const auto& [filename, doc] : validCSS)
    {
        auto path = GetCSSFilePath(filename);

        auto result = LoadFromFile(path);
        REQUIRE(result);

        INFO(filename);
        REQUIRE(*result == doc);
    }
}
