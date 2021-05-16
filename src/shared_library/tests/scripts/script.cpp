// TODO: We no longer have the Character script type in the shared project
// Think of how to test the script class.
//#include "catch2/catch.hpp"
//#include "test_util.h"
//#include "scripting/script_system.h"
//
//using namespace projectfarm::shared::scripting;
//using namespace std::literals::string_literals;
//
//TEST_CASE("CallFunction - invalid function - returns false", "[script]")
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
//    auto invalidFunctionType = static_cast<FunctionTypes>(99999);
//
//    auto result = script->CallFunction(invalidFunctionType);
//
//    REQUIRE_FALSE(result);
//}
//
//TEST_CASE("CallFunction - unknown function - returns false", "[script]")
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
//    auto functionType = FunctionTypes::UnknownFunction;
//
//    auto result = script->CallFunction(functionType);
//
//    REQUIRE_FALSE(result);
//}
//
//TEST_CASE("CallFunction - known function - returns true", "[script]")
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
//    auto functionType = FunctionTypes::Update;
//
//    auto result = script->CallFunction(functionType);
//
//    REQUIRE(result);
//}