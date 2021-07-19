#include <filesystem>

#include "catch2/catch.hpp"
#include "test_util.h"
#include "api/config/reader.h"

using namespace std::literals;
using namespace projectfarm::shared::api::config;

/*********************************************
 * Read
 ********************************************/

TEST_CASE("Read - invalid path - returns error", "[api/config]")
{
    std::filesystem::path path = "";

    auto result = Read(path);

    REQUIRE_FALSE(result);
}

TEST_CASE("Read - invalid format - returns error", "[api/config]")
{
    std::filesystem::path path = GetConfigFilePath("invalid_content.txt");

    auto result = Read(path);

    REQUIRE_FALSE(result);
}

TEST_CASE("Read - valid format - does not return error", "[api/config]")
{
    std::filesystem::path path = GetConfigFilePath("valid_content.json");

    auto result = Read(path);

    REQUIRE(result);
}

TEST_CASE("Read - valid format - reads data", "[api/config]")
{
    ItemRoot root1;
    root1.Children.push_back({ "valid", "true", {}, });

    ItemRoot root2;
    root2.Children.push_back({ "key1", "value1", {}, });
    root2.Children.push_back({ "key2", "",
                               { { "0", "", {
                                   { "key3", "true" },
                                   { "key4", "false" },
                                   { "key5", "1" },
                                   { "key6", "2.3" },
                               } } }, });

    std::vector<std::pair<std::string, ItemRoot>> data
    {
        //{ "valid_content.json", root1 },
        { "valid_content_2.json", root2 },
    };

    for (const auto& [path, expected] : data)
    {
        auto result = Read(GetConfigFilePath(path));

        REQUIRE(result == expected);
    }
}
