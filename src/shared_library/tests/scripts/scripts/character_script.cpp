// TODO: This test needs to move to the server project where the
// Character script class now resides
//#include "catch2/catch.hpp"
//#include "test_util.h"
//#include "scripting/script_system.h"
//
//using namespace projectfarm::shared::scripting;
//using namespace std::literals::string_literals;
//
//TEST_CASE("GetFunctions - returns correct functions", "[character_script]")
//{
//    auto scriptSystem = std::make_shared<ScriptSystem>();
//
//    auto type = ScriptTypes::Character;
//    auto code = "function update(){}"s;
//
//    if (!scriptSystem->Initialize(CurrentWorkingDirectory))
//    {
//        FAIL("Failed to initialize script system.");
//    }
//
//    auto script = scriptSystem->CreateScript(type, code);
//
//    auto result = script->GetFunctions();
//
//    scriptSystem->Shutdown();
//
//    auto expected = { FunctionTypes::Update };
//
//    REQUIRE(expected.size() == result.size());
//    auto r = result.begin();
//    auto e = expected.begin();
//    while (r != result.end())
//    {
//        REQUIRE(*r == *e);
//        ++r;
//        ++e;
//    }
//}
//
//TEST_CASE("Construct - Has all functions - successfully constructs", "[character_script]")
//{
//    auto scriptSystem = std::make_shared<ScriptSystem>();
//
//    auto type = ScriptTypes::Character;
//    auto code = "function update(){}"s;
//
//    if (!scriptSystem->Initialize(CurrentWorkingDirectory))
//    {
//        FAIL("Failed to initialize script system.");
//    }
//
//    auto result = scriptSystem->CreateScript(type, code);
//
//    REQUIRE(result != nullptr);
//}
//
//TEST_CASE("Construct - Missing 'update' function - does not construct", "[character_script]")
//{
//    auto scriptSystem = std::make_shared<ScriptSystem>();
//
//    auto type = ScriptTypes::Character;
//    auto code = "function not_update(){}"s;
//
//    if (!scriptSystem->Initialize(CurrentWorkingDirectory))
//    {
//        FAIL("Failed to initialize script system.");
//    }
//
//    auto result = scriptSystem->CreateScript(type, code);
//
//    REQUIRE(result == nullptr);
//}