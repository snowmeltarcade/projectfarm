#include <fstream>

#include "plots.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::world
{
    bool Plots::Load(const std::filesystem::path& worldName)
    {
        auto plotsFileName = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::SharedWorlds,
            worldName / "plots.json");

        std::ifstream file(plotsFileName);

        if (!file.is_open())
        {
            shared::api::logging::Log("Failed to open file: " + plotsFileName.u8string());
            return false;
        }

        nlohmann::json jsonFile;
        file >> jsonFile;

        auto plotsJson = jsonFile["plots"];

        for (const auto& plotJson : plotsJson)
        {
            auto plot = std::make_shared<Plot>();
            plot->SetTileSetPool(this->_tileSetPool);

            if (!plot->LoadFromJson(plotJson))
            {
                shared::api::logging::Log("Failed to load plot with json: " + plotJson.dump());
                return false;
            }

            this->_plots.emplace_back(std::move(plot));
        }

        return true;
    }

    void Plots::Shutdown() noexcept
    {
        this->_plots.clear();
    }

    uint16_t Plots::GetIndexByName(const std::string& name) const noexcept
    {
        if (name.empty())
        {
            return Plots::EmptyIndex;
        }

        for (auto index = 0u; index < this->_plots.size(); ++index)
        {
            if (name == this->_plots[index]->GetName())
            {
                return static_cast<uint16_t>(index);
            }
        }

        shared::api::logging::Log("Failed to find plot with name: " + name);

        return Plots::EmptyIndex;
    }
}