#include <algorithm>

#include "island.h"
#include "utils/util.h"

namespace projectfarm::engine::world
{
    bool Island::LoadFromJson(const nlohmann::json& jsonFile,
            const std::shared_ptr<Plots>& plots)
    {
        this->_plots = plots;

        auto jsonIt = jsonFile.find("x");
        this->_positionX = jsonIt == jsonFile.end() ? 0.0f : jsonIt->get<float>();

        jsonIt = jsonFile.find("y");
        this->_positionY = jsonIt == jsonFile.end() ? 0.0f : jsonIt->get<float>();

        this->_widthInTiles = jsonFile["widthInTiles"].get<uint32_t>();
        this->_heightInTiles = jsonFile["heightInTiles"].get<uint32_t>();

        this->_tileWidth = jsonFile["tileWidth"].get<float>();
        this->_tileHeight = jsonFile["tileHeight"].get<float>();

        this->_widthInMeters = this->_widthInTiles * this->_tileWidth;
        this->_heightInMeters = this->_heightInTiles * this->_tileHeight;

        this->_tileMap = std::make_shared<graphics::TileMap>();
        this->_tileMap->SetLogger(this->_logger);
        this->_tileMap->SetDataProvider(this->_dataProvider);
        this->_tileMap->SetGraphics(this->GetGraphics());
        this->_tileMap->SetRenderManager(this->_renderManager);
        this->_tileMap->SetTimer(this->GetTimer());

        auto layersJson = jsonFile["layers"];

        auto layers = this->BuildLayers(layersJson);

        auto tileWidthInPixels = static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_tileWidth));
        auto tileHeightInPixels = static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_tileHeight));

        if (!this->_tileMap->LoadDirect(static_cast<uint8_t>(layers.size()),
                                        this->_widthInTiles, this->_heightInTiles,
                                        tileWidthInPixels, tileHeightInPixels))
        {
            this->LogMessage("Failed to load tilemap.");
            return false;
        }

        this->_tileMap->SetRenderToWorldSpace(this->_renderToWorldSpace);

        this->ReconfirmPixelSizes();

        if (!this->RefreshTileSet(layers))
        {
            this->LogMessage("Failed to refresh tile set.");
            return false;
        }

        return true;
    }

    bool Island::LoadFromBinary(std::ifstream& fs, const std::shared_ptr<Plots>& plots) noexcept
    {
        this->_plots = plots;

        this->_positionX = 0.0f;
        this->_positionY = 0.0f;

        this->_tileWidth = 1.0f;
        this->_tileHeight = 1.0f;

        this->_widthInTiles = pfu::ReadUInt32FromBinaryFile(fs);
        this->_heightInTiles = pfu::ReadUInt32FromBinaryFile(fs);

        this->_widthInMeters = this->_widthInTiles * this->_tileWidth;
        this->_heightInMeters = this->_heightInTiles * this->_tileHeight;

        auto layers = this->BuildLayers(fs);

        this->_tileMap = std::make_shared<graphics::TileMap>();
        this->_tileMap->SetLogger(this->_logger);
        this->_tileMap->SetDataProvider(this->_dataProvider);
        this->_tileMap->SetGraphics(this->GetGraphics());
        this->_tileMap->SetRenderManager(this->_renderManager);
        this->_tileMap->SetTimer(this->GetTimer());

        auto tileWidthInPixels = static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_tileWidth));
        auto tileHeightInPixels = static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_tileHeight));

        if (!this->_tileMap->LoadDirect(static_cast<uint8_t>(layers.size()),
                                        this->_widthInTiles, this->_heightInTiles,
                                        tileWidthInPixels, tileHeightInPixels))
        {
            this->LogMessage("Failed to load tilemap.");
            return false;
        }

        this->_tileMap->SetRenderToWorldSpace(this->_renderToWorldSpace);

        this->ReconfirmPixelSizes();

        if (!this->RefreshTileSet(layers))
        {
            this->LogMessage("Failed to refresh tile set.");
            return false;
        }

        return true;
    }

    void Island::Shutdown()
    {
        this->_tileMap->Shutdown();
    }

    void Island::Tick()
    {
        this->_tileMap->Tick();
    }

    void Island::Render() noexcept
    {
        this->_tileMap->SetRenderDetails(this->_renderX, this->_renderY);

        this->_tileMap->Render();
    }

    void Island::RenderGround() noexcept
    {
        this->_tileMap->SetRenderDetails(this->_renderX, this->_renderY);

        this->_tileMap->RenderGroup(0);
    }

    void Island::RenderOverhead() noexcept
    {
        this->_tileMap->SetRenderDetails(this->_renderX, this->_renderY);

        this->_tileMap->RenderGroup(1);
    }

    void Island::RenderRegionToLayer(uint16_t plotIndex,
                                     uint32_t x, uint32_t y,
                                     uint32_t w, uint32_t h,
                                     Island::LayerType& layer) const noexcept
    {
        for (auto yPos = y; yPos < y + h; ++yPos)
        {
            if (yPos >= this->_heightInTiles)
            {
                break;
            }

            for (auto xPos = x; xPos < x + w; ++xPos)
            {
                if (xPos >= this->_widthInTiles)
                {
                    break;
                }

                layer[yPos][xPos] = plotIndex;
            }
        }
    }

    Island::LayersType Island::BuildLayers(const nlohmann::json& layersJson)
    {
        Island::LayersType layers;

        for (const auto& layerJson : layersJson)
        {
            auto defaultPlotIter = layerJson.find("defaultPlot");
            auto defaultPlot = defaultPlotIter == layerJson.end() ? "" : (*defaultPlotIter).get<std::string>();

            auto layer = this->CreateLayer(defaultPlot);

            auto regionsJson = layerJson.find("regions");
            if (regionsJson != layerJson.end())
            {
                for (const auto& regionJson : *regionsJson)
                {
                    auto name = regionJson["name"].get<std::string>();
                    auto x = regionJson["x"].get<uint32_t>();
                    auto y = regionJson["y"].get<uint32_t>();
                    auto w = regionJson["w"].get<uint32_t>();
                    auto h = regionJson["h"].get<uint32_t>();

                    auto plotIndex = this->_plots->GetIndexByName(name);

                    this->RenderRegionToLayer(plotIndex, x, y, w, h, layer);
                }
            }

            auto plotsJson = layerJson["plots"];

            for (const auto& plotJson : plotsJson)
            {
                auto name = plotJson["name"].get<std::string>();
                auto x = plotJson["x"].get<uint32_t>();
                auto y = plotJson["y"].get<uint32_t>();

                auto plotIndex = this->_plots->GetIndexByName(name);

                layer[y][x] = plotIndex;
            }

            layers.push_back({false, std::move(layer)});
        }

        return layers;
    }

    Island::LayersType Island::BuildLayers(std::ifstream& fs) const noexcept
    {
        auto numberOfLayers = pfu::ReadUInt32FromBinaryFile(fs);

        Island::LayersType layers;
        layers.reserve(numberOfLayers);

        for (auto i = 0u; i < numberOfLayers; ++i)
        {
            auto isOverhead = pfu::ReadBoolFromBinaryFile(fs);

            auto layer = this->CreateLayer(fs);

            layers.push_back({isOverhead, std::move(layer)});
        }

        return layers;
    }

    Island::LayerType Island::CreateLayer(const std::string& plotName) const noexcept
    {
        Island::LayerType layer;

        auto plotIndex = this->_plots->GetIndexByName(plotName);

        for (auto y = 0u; y < this->_heightInTiles; ++y)
        {
            std::vector<uint16_t> row;
            row.reserve(this->_widthInTiles);

            for (auto x = 0u; x < this->_widthInTiles; ++x)
            {
                row.push_back(plotIndex);
            }

            layer.emplace_back(std::move(row));
        }

        return layer;
    }

    Island::LayerType Island::CreateLayer(std::ifstream& fs) const noexcept
    {
        Island::LayerType layer;

        for (auto y = 0u; y < this->_heightInTiles; ++y)
        {
            std::vector<uint16_t> row;
            row.reserve(this->_widthInTiles);

            for (auto x = 0u; x < this->_widthInTiles; ++x)
            {
                auto plotIndex = pfu::ReadInt32FromBinaryFile(fs);
                if (plotIndex == -1)
                {
                    plotIndex = Plots::EmptyIndex;
                }

                row.push_back(static_cast<uint16_t>(plotIndex));
            }

            layer.emplace_back(std::move(row));
        }

        return layer;
    }


    bool Island::IndexTileSets()
    {
        std::vector<std::string> tileSetsIndexList;

        this->_tileMapIndexes.clear();

        for (const auto& plot : this->_plots->GetPlots())
        {
            for (auto i = 0u; i < plot->GetNumberOfFrames(); ++i)
            {
                auto tileSetName = plot->GetTileSet(i);
                auto iter = std::find(tileSetsIndexList.begin(), tileSetsIndexList.end(), tileSetName);

                auto index = std::distance(tileSetsIndexList.begin(), iter);

                if (iter == tileSetsIndexList.end())
                {
                    auto tileSet = this->_tileSetPool->Get(tileSetName);

                    if (!this->_tileMap->SetTileSet(static_cast<uint8_t>(tileSetsIndexList.size()), tileSet))
                    {
                        this->LogMessage("Failed to set tileset");
                        return false;
                    }

                    tileSetsIndexList.push_back(tileSetName);
                    index = tileSetsIndexList.size() - 1;
                }

                // store the tileset index against the name of the plot
                this->_tileMapIndexes[plot->GetName()] = static_cast<uint8_t>(index);
            }
        }

        return true;
    }

    bool Island::RefreshTileSet(const Island::LayersType& layers)
    {
        if (!this->IndexTileSets())
        {
            this->LogMessage("Failed to index tile sets.");
            return false;
        }

        for (auto layerIndex = 0u; layerIndex < layers.size(); ++layerIndex)
        {
            auto& [_, layer] = layers[layerIndex];

            for (auto y = 0u; y < this->_heightInTiles; ++y)
            {
                for (auto x = 0u; x < this->_widthInTiles; ++x)
                {
                    const auto& plotIndex = layer[y][x];

                    if (plotIndex == Plots::EmptyIndex)
                    {
                        continue;
                    }

                    if (!this->UpdatePlotOnTileMap(plotIndex, static_cast<uint8_t>(layerIndex), x, y))
                    {
                        this->LogMessage("Failed to update plot on tilemap.");
                        return false;
                    }
                }
            }
        }

        for (auto layerIndex = 0u; layerIndex < layers.size(); ++layerIndex)
        {
            auto& [isOverhead, _] = layers[layerIndex];

            auto groupIndex = static_cast<uint8_t>(isOverhead ? 1u : 0u);
            this->_tileMap->SetLayerGroup(static_cast<uint8_t>(layerIndex), groupIndex);
        }

        return true;
    }

    auto Island::UpdatePlotOnTileMap(uint16_t plotIndex, uint8_t layer,
                                     uint32_t x, uint32_t y) noexcept -> bool
    {
        auto& plot = this->_plots->GetByIndex(plotIndex);

        if (plot->GetNumberOfFrames() > 1)
        {
            auto animationData = this->GetAnimationInfoFromPlot(plot);

            if (!this->_tileMap->SetTileIndex(layer, x, y,
                                              animationData))
            {
                this->LogMessage("Failed to set tile index.");
                return false;
            }
        }
        else
        {
            auto tileSetId { static_cast<uint8_t>(this->_tileMapIndexes[plot->GetName()]) };
            if (!this->_tileMap->SetTileIndex(tileSetId,
                                              layer, x, y,
                                              plot->GetAbsoluteIndex(0)))
            {
                this->LogMessage("Failed to set tile index.");
                return false;
            }
        }

        auto tileIndex = this->_tileMap->GetTile(layer, x, y);
        this->_tileMapPlotMap[tileIndex] = plotIndex;

        return true;
    }

    auto Island::GetAnimationInfoFromPlot(const std::shared_ptr<Plot>& plot) noexcept
        -> std::vector<graphics::TileMapTileAnimationData>
    {
        std::vector<graphics::TileMapTileAnimationData> animationData;

        for (auto animationIndex = 0u; animationIndex < plot->GetNumberOfFrames(); ++animationIndex)
        {
            graphics::TileMapTileAnimationData data;

            auto tileSetIndex = this->_tileMapIndexes[plot->GetTileSet(animationIndex)];

            data.Set(tileSetIndex,
                     plot->GetAbsoluteIndex(animationIndex),
                     plot->GetMilliseconds(animationIndex));

            animationData.emplace_back(data);
        }

        return animationData;
    }

    void Island::ReconfirmPixelSizes() noexcept
    {
        auto tileWidthInPixels = static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_tileWidth));
        auto tileHeightInPixels = static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_tileHeight));

        this->_tileMap->SetRenderDetails(this->_renderX, this->_renderY,
                                         tileWidthInPixels * this->_widthInTiles,
                                         tileHeightInPixels * this->_heightInTiles,
                                         graphics::RenderOriginPoints::TopLeft);

        this->_tileMap->SetTileSize(tileWidthInPixels, tileHeightInPixels);

        // islands never move, so only need to do this when pixels per meter may change
        this->_renderX = this->GetGraphics()->MetersToPixels(this->_positionX);
        this->_renderY = this->GetGraphics()->MetersToPixels(this->_positionY);
    }

    void Island::SetRenderBounds(const SDL_Rect& viewport) noexcept
    {
        auto tileX = (viewport.x - this->_renderX) / this->GetGraphics()->MetersToPixels(this->_tileWidth);
        auto tileY = (viewport.y - this->_renderY) / this->GetGraphics()->MetersToPixels(this->_tileHeight);

        tileX = std::max(tileX, 0);
        tileY = std::max(tileY, 0);

        auto widthInTiles = static_cast<uint32_t>(viewport.w / this->GetGraphics()->MetersToPixels(this->_tileWidth));
        auto heightInTiles = static_cast<uint32_t>(viewport.h / this->GetGraphics()->MetersToPixels(this->_tileHeight));

        widthInTiles = std::min(widthInTiles + 1, this->_widthInTiles);
        heightInTiles = std::min(heightInTiles + 1, this->_heightInTiles);

        this->SetRenderBounds(static_cast<uint32_t>(tileX), static_cast<uint32_t>(tileY),
                              widthInTiles, heightInTiles);
    }

    void Island::ProcessPlotUpdate(uint8_t layerIndex, uint32_t tileX, uint32_t tileY,
                                   uint64_t updateTime, uint16_t plotIndex) noexcept
    {
        this->_lastPlotUpdateTime = updateTime;

        if (!this->UpdatePlotOnTileMap(plotIndex, layerIndex, tileX, tileY))
        {
            this->LogMessage("Failed to update plot.");
        }
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

        auto tileX = static_cast<uint32_t>(dx / this->_tileWidth);
        auto tileY = static_cast<uint32_t>(dy / this->_tileHeight);

        auto topLayer = this->_tileMap->GetNumberOfLayers();

        while (topLayer--)
        {
            auto index = this->_tileMap->GetTile(topLayer, tileX, tileY);

            if (index == Plots::EmptyIndex)
            {
                continue;
            }

            return this->_tileMapPlotMap.at(index);
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

        auto tileX = static_cast<uint32_t>(dx / this->_tileWidth);
        auto tileY = static_cast<uint32_t>(dy / this->_tileHeight);

        return {tileX, tileY};
    }
}