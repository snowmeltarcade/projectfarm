#include <nlohmann/json.hpp>
#include <fstream>

#include "character_appearance_library.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::entities
{
    bool CharacterAppearanceLibrary::Setup() noexcept
    {
        auto locations = this->_dataProvider->GetCharacterActionAnimationLocations();

        for (const auto& [_, path] : locations)
        {
            if (!this->AddToLibrary(path))
            {
                shared::api::logging::Log("Failed to add character appearance file to library: " + path.u8string());
                continue;
            }
        }

        return true;
    }

    bool CharacterAppearanceLibrary::AddToLibrary(const std::filesystem::path& path) noexcept
    {
        try
        {
            std::ifstream fs(path);

            if (!fs)
            {
                shared::api::logging::Log("Failed to open file: " + path.u8string());
                return false;
            }

            nlohmann::json jsonFile;
            fs >> jsonFile;

            auto type = jsonFile["type"].get<std::string>();
            auto part = jsonFile["part"].get<std::string>();

            this->_library[type][part].push_back(path);
        }
        catch (const std::exception& ex)
        {
            shared::api::logging::Log("Failed to add character appearance file: '" + path.u8string() +
                             "' to library with error: " + ex.what());
            return false;
        }

        return true;
    }
}