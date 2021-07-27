#ifndef PROJECTFARM_PROVIDER_H
#define PROJECTFARM_PROVIDER_H

#include <filesystem>
#include <unordered_map>
#include <string>

#include "locations.h"

namespace projectfarm::shared::api::data
{
    [[nodiscard]]
    bool SetupClient() noexcept;

    [[nodiscard]]
    bool SetupServer() noexcept;

    void SetDataFolderPath(const std::filesystem::path& basePath) noexcept;

    [[nodiscard]]
    std::filesystem::path ResolveFileName(Locations location, const std::filesystem::path& fileName) noexcept;

    [[nodiscard]]
    std::filesystem::path NormalizePath(const std::filesystem::path& path) noexcept;

    [[nodiscard]]
    const std::filesystem::path& GetTileSetPathFromName(const std::string& name) noexcept;

    [[nodiscard]]
    const std::filesystem::path& GetCharacterPathFromName(const std::string& name) noexcept;

    [[nodiscard]]
    const std::filesystem::path& GetCharacterActionAnimationLocationsFromName(const std::string& name) noexcept;

    [[nodiscard]]
    const std::unordered_map<std::string, std::filesystem::path>& GetCharacterActionAnimationLocations() noexcept;

    [[nodiscard]]
    const std::filesystem::path& GetUILocationFromName(const std::string& name) noexcept;

    [[nodiscard]]
    const std::filesystem::path& GetUICustomControlLocationFromName(const std::string& name) noexcept;

    [[nodiscard]]
    const std::unordered_map<std::string, std::filesystem::path>& GetGraphicsMaterialLocations() noexcept;

    [[nodiscard]]
    const std::filesystem::path& GetGraphicsMaterialLocationFromName(const std::string& name) noexcept;

    [[nodiscard]]
    const std::unordered_map<std::string, std::filesystem::path>& GetGraphicsShaderLocations() noexcept;

    [[nodiscard]]
    const std::filesystem::path& GetGraphicsShaderLocationByName(const std::string& name) noexcept;

    [[nodiscard]]
    const std::unordered_map<std::string, std::filesystem::path>& GetDefaultCSSLocations() noexcept;

    [[nodiscard]]
    std::filesystem::path GetDevDirectoryPath() noexcept;

    [[nodiscard]]
    std::filesystem::path GetClientDirectoryPath() noexcept;

    [[nodiscard]]
    std::filesystem::path GetServerDirectoryPath() noexcept;

    [[nodiscard]]
    std::filesystem::path GetSharedDirectoryPath() noexcept;

    [[nodiscard]]
    const std::unordered_map<std::string, std::filesystem::path>& GetWorldLocations() noexcept;
}

#endif
