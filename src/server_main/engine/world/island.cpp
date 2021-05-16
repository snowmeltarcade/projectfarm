#include "island.h"
#include "utils/util.h"

using namespace projectfarm::shared::utils;

namespace projectfarm::engine::world
{
    bool Island::LoadFromJson(const nlohmann::json& json, const std::shared_ptr<Plots>& plots)
    {
        this->_plots = plots;

        auto jsonIt = json.find("x");
        this->_positionX = jsonIt == json.end() ? 0.0f : jsonIt->get<float>();

        jsonIt = json.find("y");
        this->_positionY = jsonIt == json.end() ? 0.0f : jsonIt->get<float>();

        this->_widthInTiles = json["widthInTiles"].get<uint32_t>();
        this->_heightInTiles = json["heightInTiles"].get<uint32_t>();

        this->_tileWidthInMeters = json["tileWidth"].get<float>();
        this->_tileHeightInMeters = json["tileHeight"].get<float>();

        this->_widthInMeters = this->_widthInTiles * this->_tileWidthInMeters;
        this->_heightInMeters = this->_heightInTiles * this->_tileHeightInMeters;

        auto layersJson = json["layers"];

        auto result = std::all_of(layersJson.begin(), layersJson.end(), [this](const auto& layerJson)
        {
            if (!this->LoadLayerFromJson(layerJson))
            {
                this->LogMessage("Failed to load layer with json: " + layerJson.dump());
                return false;
            }

            return true;
        });

        return result;
    }

    bool Island::LoadFromBinary(std::ifstream& fs, const std::shared_ptr<Plots>& plots) noexcept
    {
        this->_plots = plots;

        this->_positionX = 0.0f;
        this->_positionY = 0.0f;

        this->_widthInTiles = ReadUInt32FromBinaryFile(fs);
        this->_heightInTiles = ReadUInt32FromBinaryFile(fs);

        this->_tileWidthInMeters = 1.0f;
        this->_tileHeightInMeters = 1.0f;

        this->_widthInMeters = this->_widthInTiles * this->_tileWidthInMeters;
        this->_heightInMeters = this->_heightInTiles * this->_tileHeightInMeters;

        auto numberOfLayers = ReadUInt32FromBinaryFile(fs);

        for (auto i = 0u; i < numberOfLayers; ++i)
        {
            if (!this->LoadLayerFromBinary(fs))
            {
                this->LogMessage("Failed to load layer from binary.");
                return false;
            }
        }

        this->LoadActionTiles(fs);

        return true;
    }

    bool Island::LoadLayerFromJson(const nlohmann::json& json)
    {
        std::vector<std::vector<uint16_t>> layer;

        auto defaultPlotIter = json.find("defaultPlot");
        auto defaultPlot = defaultPlotIter == json.end() ? "" : (*defaultPlotIter).get<std::string>();

        layer.resize(this->_heightInTiles);

        for (auto y = 0u; y < this->_heightInTiles; ++y)
        {
            layer[y].resize(this->_widthInTiles);

            for (auto x = 0u; x < this->_widthInTiles; ++x)
            {
                if (!defaultPlot.empty())
                {
                    layer[y][x] = this->_plots->GetPlotIndexByName(defaultPlot);
                }
            }
        }

        auto regionsJson = json["regions"];

        for (const auto& regionJson : regionsJson)
        {
            auto name = regionJson["name"].get<std::string>();
            auto x = regionJson["x"].get<uint32_t>();
            auto y = regionJson["y"].get<uint32_t>();
            auto w = regionJson["w"].get<uint32_t>();
            auto h = regionJson["h"].get<uint32_t>();

            for (auto yPos = y; yPos < std::min(y + h, this->_heightInTiles); ++yPos)
            {
                for (auto xPos = x; xPos < std::min(x + w, this->_widthInTiles); ++xPos)
                {
                    layer[yPos][xPos] = this->_plots->GetPlotIndexByName(name);
                }
            }
        }

        auto plotsJson = json["plots"];

        for (const auto& plotJson : plotsJson)
        {
            auto name = plotJson["name"].get<std::string>();
            auto x = plotJson["x"].get<uint32_t>();
            auto y = plotJson["y"].get<uint32_t>();

            layer[y][x] = this->_plots->GetPlotIndexByName(name);
        }

        this->_layers.emplace_back(std::move(layer));

        return true;
    }

    bool Island::LoadLayerFromBinary(std::ifstream& fs) noexcept
    {
        std::vector<std::vector<uint16_t>> layer;

        // is overhead layer
        ReadBoolFromBinaryFile(fs);

        layer.resize(this->_heightInTiles);

        for (auto y = 0u; y < this->_heightInTiles; ++y)
        {
            layer[y].resize(this->_widthInTiles);

            for (auto x = 0u; x < this->_widthInTiles; ++x)
            {
                auto plotIndex = ReadInt32FromBinaryFile(fs);
                if (plotIndex == -1)
                {
                    plotIndex = Plots::EmptyIndex;
                }

                layer[y][x] = static_cast<uint16_t>(plotIndex);
            }
        }

        // I can't see any reason why the server needs treat overhead layers any
        // differently to non-overhead layers
        this->_layers.emplace_back(std::move(layer));

        return true;
    }

    void Island::LoadActionTiles(std::ifstream& fs) noexcept
    {
        if (!fs)
        {
            return;
        }

        auto numberOfActionTiles = ReadUInt32FromBinaryFile(fs);

        for (auto i = 0u; i < numberOfActionTiles; ++i)
        {
            ActionTile actionTile;

            actionTile.X = ReadUInt32FromBinaryFile(fs);
            actionTile.Y = ReadUInt32FromBinaryFile(fs);

            auto numberOfProperties = ReadUInt32FromBinaryFile(fs);

            for (auto p = 0u; p < numberOfProperties; ++p)
            {
                auto name = ReadStringFromBinaryFile(fs);
                auto value = ReadStringFromBinaryFile(fs);

                actionTile.Properties[name] = value;
            }

            this->_actionTiles.emplace_back(std::move(actionTile));
        }
    }

    void Island::Shutdown() noexcept
    {
    }

    uint16_t Island::GetPlotIndexAtWorldPosition(float x, float y) const noexcept
    {
        if (x < this->_positionX || x > this->_positionX + this->_widthInMeters ||
            y < this->_positionY || y > this->_positionY + this->_heightInMeters)
        {
            return Plots::EmptyIndex;
        }

        auto dx = x - this->_positionX;
        auto dy = y - this->_positionY;

        auto tileX = static_cast<uint32_t>(dx / this->_tileWidthInMeters);
        auto tileY = static_cast<uint32_t>(dy / this->_tileHeightInMeters);

        auto topLayer = this->_layers.size();

        while (topLayer--)
        {
            auto index = this->_layers[topLayer][tileY][tileX];

            if (index == Plots::EmptyIndex)
            {
                continue;
            }

            return index;
        }

        return Plots::EmptyIndex;
    }

    std::pair<int32_t, int32_t> Island::GetTileIndexesFromWorldPosition(float x, float y) const noexcept
    {
        if (x < this->_positionX || x > this->_positionX + this->_widthInMeters ||
            y < this->_positionY || y > this->_positionY + this->_heightInMeters)
        {
            return {-1, -1};
        }

        auto dx = x - this->_positionX;
        auto dy = y - this->_positionY;

        auto tileX = static_cast<uint32_t>(dx / this->_tileWidthInMeters);
        auto tileY = static_cast<uint32_t>(dy / this->_tileHeightInMeters);

        return {tileX, tileY};
    }

    std::pair<float, float> Island::GetWorldPositionFromTileCoordinate(uint32_t x, uint32_t y) const noexcept
    {
        return { this->_positionX + (x * this->_tileWidthInMeters),
                 this->_positionY + (y * this->_tileHeightInMeters)};
    }

    std::vector<ActionTile> Island::GetActionTilesByProperty(const std::string& name, const std::string& value) noexcept
    {
        std::vector<ActionTile> tiles;

        for (auto& tile : this->_actionTiles)
        {
            if (tile.Properties[name] == value)
            {
                tiles.emplace_back(tile);
            }
        }

        return tiles;
    }

    std::string Island::GetPropertyValueForTile(const std::string& key, uint32_t x, uint32_t y) const noexcept
    {
        for (auto& tile : this->_actionTiles)
        {
            if (tile.X == x && tile.Y == y)
            {
                auto value = tile.Properties.find(key);
                if (value != tile.Properties.end())
                {
                    return value->second;
                }
            }
        }

        return "";
    }
}