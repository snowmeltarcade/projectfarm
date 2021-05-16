#include <nlohmann/json.hpp>
#include <fstream>

#include "action_animations_manager.h"

using namespace std::literals;

namespace projectfarm::engine::entities
{
    bool ActionAnimationsManager::Load() noexcept
    {
        auto locations = this->_dataProvider->GetCharacterActionAnimationLocations();

        for (const auto& [name, path] : locations)
        {
            if (!this->AddToLibrary(name, path))
            {
                this->LogMessage("Failed to add character appearance file to manager: " + path.u8string());
                continue;
            }
        }

        return true;
    }

    bool ActionAnimationsManager::AddToLibrary(const std::string& name, const std::filesystem::path& path) noexcept
    {
        try
        {
            std::ifstream fs(path);

            if (!fs)
            {
                this->LogMessage("Failed to open file: " + path.u8string());
                return false;
            }

            nlohmann::json jsonFile;
            fs >> jsonFile;

            auto type = jsonFile["type"].get<std::string>();
            auto part = jsonFile["part"].get<std::string>();

            this->_typePartMap[type][part].emplace_back(name);
        }
        catch (const std::exception& ex)
        {
            this->LogMessage("Failed to add character appearance file: '" + path.u8string() +
                             "' to manager with error: " + ex.what());
            return false;
        }

        return true;
    }

    std::string ActionAnimationsManager::GetAppearance(const std::string& type, const std::string& part) const noexcept
    {
        auto appearance = "none"s;

        if (this->_typePartMap.find(type) == this->_typePartMap.end())
        {
            return appearance;
        }

        const auto& [_, parts] = *this->_typePartMap.find(type);

        if (parts.find(part) == parts.end())
        {
            return appearance;
        }

        const auto& [__, names] = *parts.find(part);

        auto index = this->_randomEngine->Next(0u, (uint32_t)names.size() - 1u);

        appearance = names[index];

        return appearance;
    }
}