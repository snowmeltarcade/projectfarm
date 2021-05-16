// TODO: We no longer have the Character script type in the shared project
// Think of how to test the script class.
//#include "catch2/catch.hpp"
//#include "test_util.h"
//#include "scripting/script_system.h"
//
//using namespace projectfarm::shared::scripting;
//using namespace std::literals::string_literals;
//
//TEST_CASE("CreateScript (path) Invalid type - returns nullptr", "[script_system]")
//{
//    auto scriptSystem = std::make_shared<ScriptSystem>();
//
//    auto invalidType = static_cast<ScriptTypes>(99999);
//    auto path = GetScriptingFilePath("character_script.js");
//
//    if (!scriptSystem->Initialize("."))
//    {
//        FAIL("Failed to initialize script system.");
//    }
//
//    auto result = scriptSystem->CreateScript(invalidType, path);
//
//    scriptSystem->Shutdown();
//
//    REQUIRE(result == nullptr);
//}
//
//TEST_CASE("CreateScript (path) Invalid file path - returns nullptr", "[script_system]")
//{
//    auto scriptSystem = std::make_shared<ScriptSystem>();
//
//    auto type = ScriptTypes::Character;
//    auto invalidPath = std::filesystem::path("I am an invalid path...");
//
//    if (!scriptSystem->Initialize(CurrentWorkingDirectory))
//    {
//        FAIL("Failed to initialize script system.");
//    }
//
//    auto result = scriptSystem->CreateScript(type, invalidPath);
//
//    scriptSystem->Shutdown();
//
//    REQUIRE(result == nullptr);
//}
//
//TEST_CASE("CreateScript (path) Valid file path, invalid code - returns nullptr", "[script_system]")
//{
//    auto scriptSystem = std::make_shared<ScriptSystem>();
//
//    auto type = ScriptTypes::Character;
//    auto path = GetScriptingFilePath("invalid_code.js");
//
//    if (!scriptSystem->Initialize(CurrentWorkingDirectory))
//    {
//        FAIL("Failed to initialize script system.");
//    }
//
//    auto result = scriptSystem->CreateScript(type, path);
//
//    scriptSystem->Shutdown();
//
//    REQUIRE(result == nullptr);
//}
//
//TEST_CASE("CreateScript (path) Valid file path, valid code - returns script", "[script_system]")
//{
//    auto scriptSystem = std::make_shared<ScriptSystem>();
//
//    auto type = ScriptTypes::Character;
//    auto path = GetScriptingFilePath("character_script.js");
//
//    if (!scriptSystem->Initialize(CurrentWorkingDirectory))
//    {
//        FAIL("Failed to initialize script system.");
//    }
//
//    auto result = scriptSystem->CreateScript(type, path);
//
//    scriptSystem->Shutdown();
//
//    REQUIRE(result != nullptr);
//}
//
//TEST_CASE("CreateScript (string) Invalid type - returns nullptr", "[script_system]")
//{
//    auto scriptSystem = std::make_shared<ScriptSystem>();
//
//    auto invalidType = static_cast<ScriptTypes>(99999);
//    auto code = "function update(){}"s;
//
//    if (!scriptSystem->Initialize(CurrentWorkingDirectory))
//    {
//        FAIL("Failed to initialize script system.");
//    }
//
//    auto result = scriptSystem->CreateScript(invalidType, code);
//
//    scriptSystem->Shutdown();
//
//    REQUIRE(result == nullptr);
//}
//
//TEST_CASE("CreateScript (string) Invalid code - returns nullptr", "[script_system]")
//{
//    auto scriptSystem = std::make_shared<ScriptSystem>();
//
//    auto type = ScriptTypes::Character;
//    auto invalidCode = "I am invalid code..."s;
//
//    if (!scriptSystem->Initialize(CurrentWorkingDirectory))
//    {
//        FAIL("Failed to initialize script system.");
//    }
//
//    auto result = scriptSystem->CreateScript(type, invalidCode);
//
//    scriptSystem->Shutdown();
//
//    REQUIRE(result == nullptr);
//}
//
//TEST_CASE("CreateScript (string) Valid code - returns script", "[script_system]")
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
//    scriptSystem->Shutdown();
//
//    REQUIRE(result != nullptr);
//}