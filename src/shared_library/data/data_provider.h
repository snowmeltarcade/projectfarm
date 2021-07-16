#ifndef PROJECTFARM_DATA_PROVIDER_H
#define PROJECTFARM_DATA_PROVIDER_H

#include <string>
#include <filesystem>
#include <unordered_map>

#include "data_provider_locations.h"

namespace projectfarm::shared
{
    class DataProvider final
    {
    public:
        explicit DataProvider(const std::filesystem::path& binaryPath)
        : _dataFolderName("data"),
          _devFolderName("dev"),
          _clientFolderName("client"),
          _sharedFolderName("shared"),
          _uiFolderName("ui"),
          _uiCSSFolderName("css"),
          _uiCSSDefaultFolderName("default"),
          _fontsFolderName("fonts"),
          _tileSetsFolderName("tilesets"),
          _tileMapsFolderName("tilemaps"),
          _worldsFolderName("worlds"),
          _serverFolderName("server"),
          _serverCharactersFolderName("characters"),
          _serverScriptsFolderName("scripts"),
          _serverScriptsCharactersFolderName("characters"),
          _serverScriptsWorldsFolderName("worlds"),
          _sharedCharactersFolderName("characters"),
          _serverDatabasesFolderName("databases"),
          _uiCustomControlsFolderName("custom_controls"),
          _clientGraphicsFolderName("graphics"),
          _clientGraphicsMaterialsFolderName("materials"),
          _clientGraphicsShadersFolderName("shaders"),
          _sharedScriptingFolderName("scripting"),
          _dataFolderPath("")
        {
            this->SetDataFolderPath(binaryPath);
        }
        ~DataProvider() = default;

        [[nodiscard]]
        bool SetupClient();

        [[nodiscard]]
        bool SetupServer();

        void Shutdown();

        [[nodiscard]]
        const std::filesystem::path& GetTileSetPathFromName(const std::string& name)
        {
            return this->_tileSetLocations[name];
        }

        [[nodiscard]]
        const std::filesystem::path& GetCharacterPathFromName(const std::string& name) noexcept
        {
          return this->_characterLocations[name];
        }

        [[nodiscard]]
        const std::filesystem::path& GetCharacterActionAnimationLocationsFromName(const std::string& name) noexcept
        {
            return this->_characterActionAnimationLocations[name];
        }

        [[nodiscard]]
        const std::unordered_map<std::string, std::filesystem::path>& GetCharacterActionAnimationLocations() const noexcept
        {
            return this->_characterActionAnimationLocations;
        }

        [[nodiscard]]
        const std::filesystem::path& GetUILocationFromName(const std::string& name) noexcept
        {
            return this->_uiLocations[name];
        }

        [[nodiscard]]
        const std::filesystem::path& GetUICustomControlLocationFromName(const std::string& name) noexcept
        {
            return this->_uiCustomControlsLocations[name];
        }

        [[nodiscard]]
        const std::unordered_map<std::string, std::filesystem::path>& GetGraphicsMaterialLocations() const noexcept
        {
            return this->_graphicsMaterialsLocations;
        }

        [[nodiscard]]
        const std::filesystem::path& GetGraphicsMaterialLocationFromName(const std::string& name) noexcept
        {
            return this->_graphicsMaterialsLocations[name];
        }

        [[nodiscard]]
        const std::unordered_map<std::string, std::filesystem::path>& GetGraphicsShaderLocations() const noexcept
        {
            return this->_graphicsShadersLocations;
        }

        [[nodiscard]]
        const std::filesystem::path& GetGraphicsShaderLocationByName(const std::string& name) noexcept
        {
            return this->_graphicsShadersLocations[name];
        }

        [[nodiscard]]
        const std::unordered_map<std::string, std::filesystem::path>& GetDefaultCSSLocations() const noexcept
        {
            return this->_defaultCSSLocations;
        }

        void SetDataFolderPath(const std::filesystem::path& basePath)
        {
            auto base = basePath;

            // this is a hack because CLion will not copy `data` into `bin`
            if (!std::filesystem::exists(basePath / this->_dataFolderName / "shared" / "worlds" / "worlds.json"))
            {
                base /= "..";
            }

            this->_dataFolderPath = base / this->_dataFolderName;
        }

        [[nodiscard]]
        std::filesystem::path GetDevDirectoryPath() const
        {
            return _dataFolderPath / _devFolderName;
        }

        [[nodiscard]]
        std::filesystem::path GetClientDirectoryPath() const
        {
            return _dataFolderPath / _clientFolderName;
        }

        [[nodiscard]]
        std::filesystem::path GetServerDirectoryPath() const
        {
            return _dataFolderPath / _serverFolderName;
        }

        [[nodiscard]]
        std::filesystem::path GetSharedDirectoryPath() const
        {
            return _dataFolderPath / _sharedFolderName;
        }

        [[nodiscard]]
        std::filesystem::path ResolveFileName(DataProviderLocations location, const std::filesystem::path& fileName);

        [[nodiscard]]
        std::filesystem::path NormalizePath(const std::filesystem::path& path);

        [[nodiscard]]
        const std::unordered_map<std::string, std::filesystem::path>& GetWorldLocations() const
        {
            return this->_worldLocations;
        }

    private:
        const std::filesystem::path _dataFolderName;
        const std::filesystem::path _devFolderName;
        const std::filesystem::path _clientFolderName;
        const std::filesystem::path _sharedFolderName;
        const std::filesystem::path _uiFolderName;
        const std::filesystem::path _uiCSSFolderName;
        const std::filesystem::path _uiCSSDefaultFolderName;
        const std::filesystem::path _fontsFolderName;
        const std::filesystem::path _tileSetsFolderName;
        const std::filesystem::path _tileMapsFolderName;
        const std::filesystem::path _worldsFolderName;
        const std::filesystem::path _serverFolderName;
        const std::filesystem::path _serverCharactersFolderName;
        const std::filesystem::path _serverScriptsFolderName;
        const std::filesystem::path _serverScriptsCharactersFolderName;
        const std::filesystem::path _serverScriptsWorldsFolderName;
        const std::filesystem::path _sharedCharactersFolderName;
        const std::filesystem::path _serverDatabasesFolderName;
        const std::filesystem::path _uiCustomControlsFolderName;
        const std::filesystem::path _clientGraphicsFolderName;
        const std::filesystem::path _clientGraphicsMaterialsFolderName;
        const std::filesystem::path _clientGraphicsShadersFolderName;
        const std::filesystem::path _sharedScriptingFolderName;

        std::filesystem::path _dataFolderPath;

        void NormalizePathForLocation(std::string& path, DataProviderLocations location);

        [[nodiscard]]
        bool LoadLocations(DataProviderLocations location,
                           const std::filesystem::path& key,
                           const std::string& fileName,
                           std::unordered_map<std::string, std::filesystem::path>& map);

        std::unordered_map<std::string, std::filesystem::path> _tileSetLocations;
        std::unordered_map<std::string, std::filesystem::path> _worldLocations;
        std::unordered_map<std::string, std::filesystem::path> _characterLocations;
        std::unordered_map<std::string, std::filesystem::path> _characterActionAnimationLocations;
        std::unordered_map<std::string, std::filesystem::path> _uiLocations;
        std::unordered_map<std::string, std::filesystem::path> _uiCustomControlsLocations;
        std::unordered_map<std::string, std::filesystem::path> _graphicsMaterialsLocations;
        std::unordered_map<std::string, std::filesystem::path> _graphicsShadersLocations;
        std::unordered_map<std::string, std::filesystem::path> _defaultCSSLocations;
    };
}

#endif
