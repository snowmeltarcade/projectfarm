#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>

#include "tile_map.h"
#include "tile_set_pool.h"
#include "graphics.h"
#include "time/timer.h"
#include "api/logging/logging.h"

namespace projectfarm::graphics
{
    bool TileMap::Load(const std::filesystem::path& filePath)
    {
        shared::api::logging::Log("Loading tile map from " + filePath.u8string());

        std::ifstream file(filePath);

        if (!file.is_open())
        {
            return false;
        }

        nlohmann::json jsonFile;

        file >> jsonFile;

        auto numberOfLayers = jsonFile["numberOfLayers"].get<uint8_t>();
        auto widthInTiles = jsonFile["widthInTiles"].get<uint32_t>();
        auto heightInTiles = jsonFile["heightInTiles"].get<uint32_t>();
        auto tileWidth = jsonFile["tileWidth"].get<uint32_t>();
        auto tileHeight = jsonFile["tileHeight"].get<uint32_t>();

        if (!this->LoadDirect(numberOfLayers, widthInTiles, heightInTiles, tileWidth, tileHeight))
        {
            shared::api::logging::Log("Failed to load tilemap from " + filePath.u8string());
            this->Shutdown();
            return false;
        }

        auto tileSets = jsonFile["tileSets"];

        for (const auto& tileSet : tileSets)
        {
            if (!this->LoadTileSet(tileSet))
            {
                shared::api::logging::Log("Failed to load tileset: " + tileSet.dump());
                this->Shutdown();
                return false;
            }
        }

        auto tiles = jsonFile["tiles"];

        for (const auto& tile : tiles)
        {
            if (!this->LoadTile(tile))
            {
                shared::api::logging::Log("Failed to load tile: " + tile.dump());
                this->Shutdown();
                return false;
            }
        }

        this->_isLoaded = true;

        shared::api::logging::Log("Loaded tile map.");

        return true;
    }

    void TileMap::Shutdown()
    {
        shared::api::logging::Log("Shutting down tile map...");

        this->_tileLayers.clear();

        this->ClearTileSets();

        this->_isLoaded = false;

        shared::api::logging::Log("Shutdown tile map.");
    }

    void TileMap::ClearTileSets() noexcept
    {
        for (const auto& kv : this->_tileSets)
        {
            if (kv.second == nullptr)
            {
                continue;
            }

            this->GetGraphics()->GetTileSetPool()->Release(kv.second->GetName());
        }
        this->_tileSets.clear();
    }

    void TileMap::Tick()
    {
        auto frameTime = this->GetTimer()->GetLastFrameDurationInMilliseconds();

        for (auto tileIndex : this->_animatedTileIndexes)
        {
            auto& tile = this->_tiles[tileIndex];
            tile->Tick(frameTime);
        }
    }

    void TileMap::Render() noexcept
    {
        for (uint8_t layerIndex = 0u; layerIndex < static_cast<uint8_t>(this->_tileLayers.size()); ++layerIndex)
        {
            this->RenderLayer(layerIndex);
        }
    }

    void TileMap::RenderGroup(uint8_t groupIndex) noexcept
    {
        auto layerIndexes = this->_layerGroups[groupIndex];

        for (auto index : layerIndexes) // no const & as this is just a list of integers
        {
            this->RenderLayer(index);
        }
    }

    void TileMap::RenderLayer(uint8_t layerIndex) noexcept
    {
        auto& layer = this->_tileLayers[layerIndex];

        auto renderLayer = this->GetGraphics()->BumpRenderLayer();

        auto x = this->_renderX + (this->_renderBoundsX * this->_tileWidth);

        auto renderWidth = this->_renderBoundsW == 0 ? this->_widthInTiles :
                           std::min(this->_renderBoundsX + this->_renderBoundsW, this->_widthInTiles);
        for (auto tileX = this->_renderBoundsX; tileX < renderWidth; ++tileX)
        {
            auto y = this->_renderY + (this->_renderBoundsY * this->_tileHeight);

            auto renderHeight = this->_renderBoundsH == 0 ? this->_heightInTiles :
                                std::min(this->_renderBoundsY + this->_renderBoundsH, this->_heightInTiles);
            for (auto tileY = this->_renderBoundsY; tileY < renderHeight; ++tileY)
            {
                auto tileIndex = layer->GetTile(tileX, tileY);

                if (tileIndex != TileMap::EmptyTileIndex)
                {
                    const auto& tile = this->_tiles[tileIndex];

                    if (!tile->IsIndexAbsoluteEmpty())
                    {
                        auto index = tile->GetIndexAbsolute();
                        auto tileSetId = tile->GetTileSetId();

                        if (this->_tileSets[tileSetId] == nullptr)
                        {
                            continue;
                        }

                        this->_tileSets[tileSetId]->RenderTile(index,
                                                               x, y,
                                                               this->_tileWidth, this->_tileHeight,
                                                               this->_renderToWorldSpace,
                                                               renderLayer);
                    }
                }

                y += this->_tileHeight;
            }

            x += this->_tileWidth;
        }
    }

    bool TileMap::LoadDirect(uint8_t numberOfLayers, uint32_t widthInTiles, uint32_t heightInTiles,
                             uint32_t tileWidth, uint32_t tileHeight)
    {
        if (this->_isLoaded)
        {
            shared::api::logging::Log("Tilemap is already loaded.");
            return false;
        }

        this->_tileLayers.reserve(numberOfLayers);

        for (auto i = 0u; i < numberOfLayers; i++)
        {
            this->_tileLayers.emplace_back(std::make_shared<TileMapTileLayer>());
            if (!this->_tileLayers[i]->Setup(widthInTiles, heightInTiles))
            {
                shared::api::logging::Log("Failed to create tilemap layer: " + std::to_string(i));
                return false;
            }
        }

        this->_widthInTiles = widthInTiles;
        this->_heightInTiles = heightInTiles;
        this->_tileWidth = tileWidth;
        this->_tileHeight = tileHeight;

        return true;
    }

    bool TileMap::SetTileSet(uint8_t tileSetId, const std::shared_ptr<TileSet>& tileSet)
    {
        if (tileSet == nullptr)
        {
            shared::api::logging::Log("Trying to set a null tileset with tileSetId: " + std::to_string(tileSetId));
            return false;
        }

        if (this->_tileSets[tileSetId] != nullptr)
        {
            return false;
        }

        this->_tileSets[tileSetId] = tileSet;

        return true;
    }

    bool TileMap::SetTileIndex(uint8_t tileSetId, uint32_t layer,
                               uint32_t tileX, uint32_t tileY, uint32_t indexAbsolute)
    {
        if (this->_tileSets.count(tileSetId) <= 0)
        {
            shared::api::logging::Log("Failed to find tile set with id: " + std::to_string(tileSetId));
            return false;
        }

        if (layer >= this->_tileLayers.size())
        {
            shared::api::logging::Log("Invalid tilemap layer id: " + std::to_string(layer));
            return false;
        }

        if (!this->_tileSets[tileSetId]->IsAbsoluteIndexValid(indexAbsolute))
        {
            shared::api::logging::Log("Invalid absolute index: " + std::to_string(tileSetId) +
                                "for tileset: " + std::to_string(indexAbsolute));
            return false;
        }

        auto tileIter = std::find_if(this->_tiles.begin(), this->_tiles.end(),
                                  [tileSetId, indexAbsolute](const auto& it)
                                     {
                                       return it->GetTileSetId() == tileSetId &&
                                              it->GetIndexAbsolute() == indexAbsolute;
                                     });

        uint16_t tileIndex {0u};

        if (tileIter == this->_tiles.end())
        {
            std::shared_ptr<TileMapTile> tile {std::make_shared<TileMapTile>(1)};

            tile->SetFrame(0, tileSetId, indexAbsolute, 0);

            this->_tiles.emplace_back(std::move(tile));

            tileIndex = static_cast<uint16_t>(this->_tiles.size() - 1);
        }
        else
        {
            tileIndex = static_cast<uint16_t>(std::distance(this->_tiles.begin(), tileIter));
        }

        this->_tileLayers[layer]->SetTile(tileX, tileY, tileIndex);

        return true;
    }

    bool TileMap::SetTileIndex(uint32_t layer, uint32_t tileX, uint32_t tileY,
                               const std::vector<TileMapTileAnimationData>& animationData)
    {
        if (layer >= this->_tileLayers.size())
        {
            shared::api::logging::Log("Invalid tilemap layer id: " + std::to_string(layer));
            return false;
        }

        auto& tileMapLayer = this->_tileLayers[layer];

        auto tileIter = std::find_if(this->_tiles.begin(), this->_tiles.end(),
                                     [animationData](const auto& it)
                                     {
                                         return *it == animationData;
                                     });

        uint16_t tileIndex {0u};

        if (tileIter == this->_tiles.end())
        {
            auto tile = std::make_shared<TileMapTile>(static_cast<uint32_t>(animationData.size()));

            auto frame = 0u;
            for (const auto& data : animationData)
            {
                auto tileSetId = data.GetTileSetId();

                if (this->_tileSets.count(tileSetId) <= 0)
                {
                    shared::api::logging::Log("Failed to find tile set with id: " + std::to_string(data.GetTileSetId()));
                    return false;
                }

                if (!this->_tileSets[tileSetId]->IsAbsoluteIndexValid(data.GetIndexAbsolute()))
                {
                    shared::api::logging::Log("Invalid absolute index: " + std::to_string(data.GetIndexAbsolute()) +
                                     " for tileset: " + std::to_string(data.GetTileSetId()));
                    return false;
                }

                if (frame >= tile->GetNumberOfFrames())
                {
                    shared::api::logging::Log("Frame: " + std::to_string(frame) +
                                     " is greater than number of frames in tile: " + std::to_string(tile->GetNumberOfFrames()));
                    return false;
                }

                tile->SetFrame(frame++, data.GetTileSetId(), data.GetIndexAbsolute(), data.GetMilliseconds());
            }

            this->_tiles.emplace_back(std::move(tile));

            tileIndex = static_cast<uint16_t>(this->_tiles.size() - 1);

            this->_animatedTileIndexes.push_back(tileIndex);
        }
        else
        {
            tileIndex = static_cast<uint16_t>(std::distance(this->_tiles.begin(), tileIter));
        }

        tileMapLayer->SetTile(tileX, tileY, tileIndex);

        return true;
    }

    bool TileMap::LoadTileSet(const nlohmann::json& tileSetJson)
    {
        auto index = tileSetJson["index"].get<uint8_t>();
        auto name = tileSetJson["name"].get<std::string>();

        auto tileSet = this->GetGraphics()->GetTileSetPool()->Get(name);
        if (tileSet == nullptr)
        {
            shared::api::logging::Log("Failed to load tileset: " + name);
            return false;
        }

        if (!this->SetTileSet(index, tileSet))
        {
            shared::api::logging::Log("Failed to set tileset with index: " + std::to_string(index));
            return false;
        }

        return true;
    }

    bool TileMap::LoadTile(const nlohmann::json& tile)
    {
        auto tileSetId = tile["tileSetId"].get<uint8_t>();
        auto layer = tile["layer"].get<uint32_t>();
        auto tileX = tile["tileX"].get<uint32_t>();
        auto tileY = tile["tileY"].get<uint32_t>();
        auto indexAbsolute = tile["indexAbsolute"].get<uint32_t>();

        if (!this->SetTileIndex(tileSetId, layer, tileX, tileY, indexAbsolute))
        {
            shared::api::logging::Log("Failed to load tile: " + tile.dump());
            return false;
        }

        return true;
    }
}