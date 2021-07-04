#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <cstdlib>

#include "data_provider.h"

namespace projectfarm::shared
{
    bool DataProvider::SetupClient()
    {
        if (!this->LoadLocations(DataProviderLocations::ClientTileSets,
                                 "tilesets.json", "tileSets",
                                 this->_tileSetLocations))
        {
            this->LogMessage("Failed to load tile set locations");
            return false;
        }

        if (!this->LoadLocations(DataProviderLocations::SharedWorlds,
                                 "worlds.json", "worlds",
                                 this->_worldLocations))
        {
            this->LogMessage("Failed to load world locations");
            return false;
        }

        if (!this->LoadLocations(DataProviderLocations::SharedCharacters,
                                 "action_animations.json", "animations",
                                 this->_characterActionAnimationLocations))
        {
            this->LogMessage("Failed to load action animations locations");
            return false;
        }

        if (!this->LoadLocations(DataProviderLocations::ClientUI,
                                 "uis.json", "uis",
                                 this->_uiLocations))
        {
            this->LogMessage("Failed to load uis locations");
            return false;
        }

        if (!this->LoadLocations(DataProviderLocations::ClientUI,
                                 "CustomControls.json", "customControls",
                                 this->_uiCustomControlsLocations))
        {
            this->LogMessage("Failed to load custom controls locations");
            return false;
        }

        if (!this->LoadLocations(DataProviderLocations::ClientGraphicsMaterials,
                                 "Materials.json", "materials",
                                 this->_graphicsMaterialsLocations))
        {
            this->LogMessage("Failed to load graphics materials locations");
            return false;
        }

        if (!this->LoadLocations(DataProviderLocations::ClientGraphicsShaders,
                                 "Shaders.json", "shaders",
                                 this->_graphicsShadersLocations))
        {
            this->LogMessage("Failed to load graphics shaders locations");
            return false;
        }

        if (!this->LoadLocations(DataProviderLocations::ClientUICSSDefault,
                                 "DefaultCSS.json", "defaultCSS",
                                 this->_defaultCSSLocations))
        {
            this->LogMessage("Failed to load graphics shaders locations");
            return false;
        }

        return true;
    }

    bool DataProvider::SetupServer()
    {
        if (!this->LoadLocations(DataProviderLocations::SharedWorlds,
                                 "worlds.json", "worlds",
                                 this->_worldLocations))
        {
            this->LogMessage("Failed to load world locations");
            return false;
        }

        if (!this->LoadLocations(DataProviderLocations::ServerCharacters,
                                 "characters.json", "characters",
                                 this->_characterLocations))
        {
            this->LogMessage("Failed to load character locations");
            return false;
        }

        if (!this->LoadLocations(DataProviderLocations::SharedCharacters,
                                 "action_animations.json", "animations",
                                 this->_characterActionAnimationLocations))
        {
            this->LogMessage("Failed to load tile set locations");
            return false;
        }

        return true;
    }

    void DataProvider::Shutdown()
    {
        this->_tileSetLocations.clear();
        this->_worldLocations.clear();
        this->_characterActionAnimationLocations.clear();
    }

    bool DataProvider::LoadLocations(DataProviderLocations location,
                                     const std::filesystem::path& fileName, const std::string& key,
                                     std::unordered_map<std::string, std::filesystem::path>& map)
    {
        map.clear();

        auto filePath = this->ResolveFileName(location, fileName);

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            this->LogMessage("Failed to load " + filePath.u8string());
            return false;
        }

        nlohmann::json jsonFile;
        file >> jsonFile;

        auto items = jsonFile[key];

        for (const auto& item : items)
        {
            auto name = item["name"].get<std::string>();
            auto path = item["filePath"].get<std::string>();

            map[name] = this->NormalizePath(path).u8string();
        }

        return true;
    }

    std::filesystem::path DataProvider::ResolveFileName(DataProviderLocations location, const std::filesystem::path& fileName)
    {
        std::filesystem::path resolvedPath = fileName;

        if (location == projectfarm::shared::DataProviderLocations::Client)
        {
            resolvedPath = this->GetClientDirectoryPath() / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientUI)
        {
            resolvedPath = this->GetClientDirectoryPath() / this->_uiFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientUICustomControls)
        {
            resolvedPath = this->GetClientDirectoryPath() / this->_uiFolderName / this->_uiCustomControlsFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientUICSSDefault)
        {
            resolvedPath = this->GetClientDirectoryPath() / this->_uiFolderName / this->_uiCSSFolderName / this->_uiCSSDefaultFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientFonts)
        {
            resolvedPath = this->GetClientDirectoryPath() / this->_fontsFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientTileSets)
        {
            resolvedPath = this->GetClientDirectoryPath() / this->_tileSetsFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientTileMaps)
        {
            resolvedPath = this->GetClientDirectoryPath() / this->_tileMapsFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientGraphicsMaterials)
        {
            resolvedPath = this->GetClientDirectoryPath() / this->_clientGraphicsFolderName /
                           this->_clientGraphicsMaterialsFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientGraphicsShaders)
        {
            resolvedPath = this->GetClientDirectoryPath() / this->_clientGraphicsFolderName /
                           this->_clientGraphicsShadersFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::SharedWorlds)
        {
            resolvedPath = this->GetSharedDirectoryPath() / this->_worldsFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::Server)
        {
            resolvedPath = this->GetServerDirectoryPath() / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ServerCharacters)
        {
            resolvedPath = this->GetServerDirectoryPath() / this->_serverCharactersFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ServerScriptsCharacters)
        {
            resolvedPath = this->GetServerDirectoryPath() / this->_serverScriptsFolderName /
                    this->_serverScriptsCharactersFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ServerScriptsWorlds)
        {
            resolvedPath = this->GetServerDirectoryPath() / this->_serverScriptsFolderName /
                           this->_serverScriptsWorldsFolderName / fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::SharedCharacters)
        {
            resolvedPath = this->GetSharedDirectoryPath() / this->_sharedCharactersFolderName /
                            fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::ServerDatabases)
        {
            resolvedPath = this->GetServerDirectoryPath() / this->_serverDatabasesFolderName /
                           fileName;
        }
        else if (location == projectfarm::shared::DataProviderLocations::SharedScripting)
        {
            resolvedPath = this->GetSharedDirectoryPath() / this->_sharedScriptingFolderName /
                           fileName;
        }

        // normalizes the path; fixes folder/../folder2
        return resolvedPath.lexically_normal();
    }

    std::filesystem::path DataProvider::NormalizePath(const std::filesystem::path& path)
    {
        std::string s = path.u8string();

        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::Client);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ClientUI);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ClientUICustomControls);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ClientUICSSDefault);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ClientFonts);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ClientTileSets);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ClientTileMaps);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ClientGraphicsMaterials);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ClientGraphicsShaders);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::SharedWorlds);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::Server);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ServerCharacters);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ServerScriptsCharacters);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ServerScriptsWorlds);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::SharedCharacters);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::ServerDatabases);
        this->NormalizePathForLocation(s, projectfarm::shared::DataProviderLocations::SharedScripting);

        std::filesystem::path p = s;
        return p.lexically_normal();
    }

    void DataProvider::NormalizePathForLocation(std::string& path, DataProviderLocations location)
    {
        std::string key = "{";

        if (location == projectfarm::shared::DataProviderLocations::Client)
        {
            key += "Client";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientUI)
        {
            key += "ClientUI";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientUICustomControls)
        {
            key += "ClientUICustomControls";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientUICSSDefault)
        {
            key += "ClientUICSSDefault";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientFonts)
        {
            key += "ClientFonts";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientTileSets)
        {
            key += "ClientTileSets";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientTileMaps)
        {
            key += "ClientTileMaps";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientGraphicsMaterials)
        {
            key += "ClientGraphicsMaterials";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ClientGraphicsShaders)
        {
            key += "ClientGraphicsShaders";
        }
        else if (location == projectfarm::shared::DataProviderLocations::SharedWorlds)
        {
            key += "SharedWorlds";
        }
        else if (location == projectfarm::shared::DataProviderLocations::Server)
        {
            key += "Server";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ServerCharacters)
        {
            key += "ServerCharacters";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ServerScriptsCharacters)
        {
            key += "ServerScriptsCharacters";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ServerScriptsWorlds)
        {
            key += "ServerScriptsWorlds";
        }
        else if (location == projectfarm::shared::DataProviderLocations::SharedCharacters)
        {
            key += "SharedCharacters";
        }
        else if (location == projectfarm::shared::DataProviderLocations::ServerDatabases)
        {
            key += "ServerDatabases";
        }
        else if (location == projectfarm::shared::DataProviderLocations::SharedScripting)
        {
            key += "SharedScripting";
        }

        key += "}";

        auto pos = path.find(key);
        if (pos == std::string::npos)
        {
            return;
        }

        auto value = this->ResolveFileName(location, "").u8string();

        path.replace(pos, key.length(), value);
    }
}
