#include <fstream>

#include "nlohmann/json.hpp"
#include "provider.h"
#include "api/logging/logging.h"

namespace projectfarm::shared::api::data
{
    std::filesystem::path DataFolderPath;
    
    const std::filesystem::path DataFolderName("data");
    const std::filesystem::path DevFolderName("dev");
    const std::filesystem::path ClientFolderName("client");
    const std::filesystem::path SharedFolderName("shared");
    const std::filesystem::path UIFolderName("ui");
    const std::filesystem::path UICSSFolderName("css");
    const std::filesystem::path UICSSDefaultFolderName("default");
    const std::filesystem::path FontsFolderName("fonts");
    const std::filesystem::path TileSetsFolderName("tilesets");
    const std::filesystem::path TileMapsFolderName("tilemaps");
    const std::filesystem::path WorldsFolderName("worlds");
    const std::filesystem::path ServerFolderName("server");
    const std::filesystem::path ServerCharactersFolderName("characters");
    const std::filesystem::path ServerScriptsFolderName("scripts");
    const std::filesystem::path ServerScriptsCharactersFolderName("characters");
    const std::filesystem::path ServerScriptsWorldsFolderName("worlds");
    const std::filesystem::path SharedCharactersFolderName("characters");
    const std::filesystem::path ServerDatabasesFolderName("databases");
    const std::filesystem::path UICustomControlsFolderName("custom_controls");
    const std::filesystem::path ClientGraphicsFolderName("graphics");
    const std::filesystem::path ClientGraphicsMaterialsFolderName("materials");
    const std::filesystem::path ClientGraphicsShadersFolderName("shaders");
    const std::filesystem::path SharedScriptingFolderName("scripting");

    std::unordered_map<std::string, std::filesystem::path> TileSetLocations;
    std::unordered_map<std::string, std::filesystem::path> WorldLocations;
    std::unordered_map<std::string, std::filesystem::path> CharacterLocations;
    std::unordered_map<std::string, std::filesystem::path> CharacterActionAnimationLocations;
    std::unordered_map<std::string, std::filesystem::path> UILocations;
    std::unordered_map<std::string, std::filesystem::path> UICustomControlsLocations;
    std::unordered_map<std::string, std::filesystem::path> GraphicsMaterialsLocations;
    std::unordered_map<std::string, std::filesystem::path> GraphicsShadersLocations;
    std::unordered_map<std::string, std::filesystem::path> DefaultCSSLocations;

    bool LoadLocations(Locations location,
                       const std::filesystem::path& fileName,
                       const std::string& key,
                       std::unordered_map<std::string, std::filesystem::path>& map) noexcept
    {
        map.clear();

        auto filePath = ResolveFileName(location, fileName);

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            api::logging::Log("Failed to load " + filePath.u8string());
            return false;
        }

        nlohmann::json jsonFile;
        file >> jsonFile;

        auto items = jsonFile[key];

        for (const auto& item : items)
        {
            auto name = item["name"].get<std::string>();
            auto path = item["filePath"].get<std::string>();

            map[name] = NormalizePath(path).u8string();
        }

        return true;
    }

    void NormalizePathForLocation(std::string& path, Locations location) noexcept
    {
        std::string key = "{";

        if (location == Locations::Client)
        {
            key += "Client";
        }
        else if (location == Locations::ClientUI)
        {
            key += "ClientUI";
        }
        else if (location == Locations::ClientUICustomControls)
        {
            key += "ClientUICustomControls";
        }
        else if (location == Locations::ClientUICSSDefault)
        {
            key += "ClientUICSSDefault";
        }
        else if (location == Locations::ClientFonts)
        {
            key += "ClientFonts";
        }
        else if (location == Locations::ClientTileSets)
        {
            key += "ClientTileSets";
        }
        else if (location == Locations::ClientTileMaps)
        {
            key += "ClientTileMaps";
        }
        else if (location == Locations::ClientGraphicsMaterials)
        {
            key += "ClientGraphicsMaterials";
        }
        else if (location == Locations::ClientGraphicsShaders)
        {
            key += "ClientGraphicsShaders";
        }
        else if (location == Locations::SharedWorlds)
        {
            key += "SharedWorlds";
        }
        else if (location == Locations::Server)
        {
            key += "Server";
        }
        else if (location == Locations::ServerCharacters)
        {
            key += "ServerCharacters";
        }
        else if (location == Locations::ServerScriptsCharacters)
        {
            key += "ServerScriptsCharacters";
        }
        else if (location == Locations::ServerScriptsWorlds)
        {
            key += "ServerScriptsWorlds";
        }
        else if (location == Locations::SharedCharacters)
        {
            key += "SharedCharacters";
        }
        else if (location == Locations::ServerDatabases)
        {
            key += "ServerDatabases";
        }
        else if (location == Locations::SharedScripting)
        {
            key += "SharedScripting";
        }

        key += "}";

        auto pos = path.find(key);
        if (pos == std::string::npos)
        {
            return;
        }

        auto value = ResolveFileName(location, "").u8string();

        path.replace(pos, key.length(), value);
    }
    
    bool SetupClient() noexcept
    {
        if (!LoadLocations(Locations::ClientTileSets,
                          "tilesets.json", "tileSets",
                          TileSetLocations))
        {
            api::logging::Log("Failed to load tile set locations");
            return false;
        }

        if (!LoadLocations(Locations::SharedWorlds,
                           "worlds.json", "worlds",
                           WorldLocations))
        {
            api::logging::Log("Failed to load world locations");
            return false;
        }

        if (!LoadLocations(Locations::SharedCharacters,
                           "action_animations.json", "animations",
                           CharacterActionAnimationLocations))
        {
            api::logging::Log("Failed to load action animations locations");
            return false;
        }

        if (!LoadLocations(Locations::ClientUI,
                           "uis.json", "uis",
                           UILocations))
        {
            api::logging::Log("Failed to load uis locations");
            return false;
        }

        if (!LoadLocations(Locations::ClientUI,
                           "CustomControls.json", "customControls",
                           UICustomControlsLocations))
        {
            api::logging::Log("Failed to load custom controls locations");
            return false;
        }

        if (!LoadLocations(Locations::ClientGraphicsMaterials,
                           "Materials.json", "materials",
                           GraphicsMaterialsLocations))
        {
            api::logging::Log("Failed to load graphics materials locations");
            return false;
        }

        if (!LoadLocations(Locations::ClientGraphicsShaders,
                           "Shaders.json", "shaders",
                           GraphicsShadersLocations))
        {
            api::logging::Log("Failed to load graphics shaders locations");
            return false;
        }

        if (!LoadLocations(Locations::ClientUICSSDefault,
                           "DefaultCSS.json", "defaultCSS",
                           DefaultCSSLocations))
        {
            api::logging::Log("Failed to load graphics shaders locations");
            return false;
        }

        return true;
    }

    bool SetupServer() noexcept
    {
        if (!LoadLocations(Locations::SharedWorlds,
                           "worlds.json", "worlds",
                           WorldLocations))
        {
            api::logging::Log("Failed to load world locations");
            return false;
        }

        if (!LoadLocations(Locations::ServerCharacters,
                           "characters.json", "characters",
                           CharacterLocations))
        {
            api::logging::Log("Failed to load character locations");
            return false;
        }

        if (!LoadLocations(Locations::SharedCharacters,
                           "action_animations.json", "animations",
                           CharacterActionAnimationLocations))
        {
            api::logging::Log("Failed to load tile set locations");
            return false;
        }

        return true;
    }
    
    void SetDataFolderPath(const std::filesystem::path& basePath) noexcept
    {
        auto base = basePath;

        // this is a hack because CLion will not copy `data` into `bin`
        if (!std::filesystem::exists(basePath / DataFolderName / "shared" / "worlds" / "worlds.json"))
        {
            base /= "..";
        }

        DataFolderPath = base / DataFolderName;
    }

    std::filesystem::path ResolveFileName(Locations location, const std::filesystem::path& fileName) noexcept
    {
        std::filesystem::path resolvedPath = fileName;

        if (location == Locations::Client)
        {
            resolvedPath = GetClientDirectoryPath() / fileName;
        }
        else if (location == Locations::ClientUI)
        {
            resolvedPath = GetClientDirectoryPath() / UIFolderName / fileName;
        }
        else if (location == Locations::ClientUICustomControls)
        {
            resolvedPath = GetClientDirectoryPath() / UIFolderName / UICustomControlsFolderName / fileName;
        }
        else if (location == Locations::ClientUICSSDefault)
        {
            resolvedPath = GetClientDirectoryPath() / UIFolderName / UICSSFolderName / UICSSDefaultFolderName / fileName;
        }
        else if (location == Locations::ClientFonts)
        {
            resolvedPath = GetClientDirectoryPath() / FontsFolderName / fileName;
        }
        else if (location == Locations::ClientTileSets)
        {
            resolvedPath = GetClientDirectoryPath() / TileSetsFolderName / fileName;
        }
        else if (location == Locations::ClientTileMaps)
        {
            resolvedPath = GetClientDirectoryPath() / TileMapsFolderName / fileName;
        }
        else if (location == Locations::ClientGraphicsMaterials)
        {
            resolvedPath = GetClientDirectoryPath() / ClientGraphicsFolderName /
                           ClientGraphicsMaterialsFolderName / fileName;
        }
        else if (location == Locations::ClientGraphicsShaders)
        {
            resolvedPath = GetClientDirectoryPath() / ClientGraphicsFolderName /
                           ClientGraphicsShadersFolderName / fileName;
        }
        else if (location == Locations::SharedWorlds)
        {
            resolvedPath = GetSharedDirectoryPath() / WorldsFolderName / fileName;
        }
        else if (location == Locations::Server)
        {
            resolvedPath = GetServerDirectoryPath() / fileName;
        }
        else if (location == Locations::ServerCharacters)
        {
            resolvedPath = GetServerDirectoryPath() / ServerCharactersFolderName / fileName;
        }
        else if (location == Locations::ServerScriptsCharacters)
        {
            resolvedPath = GetServerDirectoryPath() / ServerScriptsFolderName /
                           ServerScriptsCharactersFolderName / fileName;
        }
        else if (location == Locations::ServerScriptsWorlds)
        {
            resolvedPath = GetServerDirectoryPath() / ServerScriptsFolderName /
                           ServerScriptsWorldsFolderName / fileName;
        }
        else if (location == Locations::SharedCharacters)
        {
            resolvedPath = GetSharedDirectoryPath() / SharedCharactersFolderName /
                           fileName;
        }
        else if (location == Locations::ServerDatabases)
        {
            resolvedPath = GetServerDirectoryPath() / ServerDatabasesFolderName /
                           fileName;
        }
        else if (location == Locations::SharedScripting)
        {
            resolvedPath = GetSharedDirectoryPath() / SharedScriptingFolderName /
                           fileName;
        }

        // normalizes the path; fixes folder/../folder2
        return resolvedPath.lexically_normal();
    }

    std::filesystem::path NormalizePath(const std::filesystem::path& path) noexcept
    {
        std::string s = path.u8string();

        NormalizePathForLocation(s, Locations::Client);
        NormalizePathForLocation(s, Locations::ClientUI);
        NormalizePathForLocation(s, Locations::ClientUICustomControls);
        NormalizePathForLocation(s, Locations::ClientUICSSDefault);
        NormalizePathForLocation(s, Locations::ClientFonts);
        NormalizePathForLocation(s, Locations::ClientTileSets);
        NormalizePathForLocation(s, Locations::ClientTileMaps);
        NormalizePathForLocation(s, Locations::ClientGraphicsMaterials);
        NormalizePathForLocation(s, Locations::ClientGraphicsShaders);
        NormalizePathForLocation(s, Locations::SharedWorlds);
        NormalizePathForLocation(s, Locations::Server);
        NormalizePathForLocation(s, Locations::ServerCharacters);
        NormalizePathForLocation(s, Locations::ServerScriptsCharacters);
        NormalizePathForLocation(s, Locations::ServerScriptsWorlds);
        NormalizePathForLocation(s, Locations::SharedCharacters);
        NormalizePathForLocation(s, Locations::ServerDatabases);
        NormalizePathForLocation(s, Locations::SharedScripting);

        std::filesystem::path p = s;
        return p.lexically_normal();
    }
    
    const std::filesystem::path& GetTileSetPathFromName(const std::string& name) noexcept
    {
        return TileSetLocations[name];
    }

    const std::filesystem::path& GetCharacterPathFromName(const std::string& name) noexcept
    {
        return CharacterLocations[name];
    }

    const std::filesystem::path& GetCharacterActionAnimationLocationsFromName(const std::string& name) noexcept
    {
        return CharacterActionAnimationLocations[name];
    }

    const std::unordered_map<std::string, std::filesystem::path>& GetCharacterActionAnimationLocations() noexcept
    {
        return CharacterActionAnimationLocations;
    }

    const std::filesystem::path& GetUILocationFromName(const std::string& name) noexcept
    {
        return UILocations[name];
    }

    const std::filesystem::path& GetUICustomControlLocationFromName(const std::string& name) noexcept
    {
        return UICustomControlsLocations[name];
    }

    const std::unordered_map<std::string, std::filesystem::path>& GetGraphicsMaterialLocations() noexcept
    {
        return GraphicsMaterialsLocations;
    }

    const std::filesystem::path& GetGraphicsMaterialLocationFromName(const std::string& name) noexcept
    {
        return GraphicsMaterialsLocations[name];
    }

    const std::unordered_map<std::string, std::filesystem::path>& GetGraphicsShaderLocations() noexcept
    {
        return GraphicsShadersLocations;
    }

    const std::filesystem::path& GetGraphicsShaderLocationByName(const std::string& name) noexcept
    {
        return GraphicsShadersLocations[name];
    }

    const std::unordered_map<std::string, std::filesystem::path>& GetDefaultCSSLocations() noexcept
    {
        return DefaultCSSLocations;
    }

    std::filesystem::path GetDevDirectoryPath() noexcept
    {
        return DataFolderPath / DevFolderName;
    }

    std::filesystem::path GetClientDirectoryPath() noexcept
    {
        return DataFolderPath / ClientFolderName;
    }

    std::filesystem::path GetServerDirectoryPath() noexcept
    {
        return DataFolderPath / ServerFolderName;
    }

    std::filesystem::path GetSharedDirectoryPath() noexcept
    {
        return DataFolderPath / SharedFolderName;
    }

    const std::unordered_map<std::string, std::filesystem::path>& GetWorldLocations() noexcept
    {
        return WorldLocations;
    }
}
