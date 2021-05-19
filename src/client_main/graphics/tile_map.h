#ifndef PROJECTFARM_TILE_MAP_H
#define PROJECTFARM_TILE_MAP_H

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "time/consume_timer.h"
#include "consume_render_manager.h"
#include "tile_map_tile_layer.h"
#include "renderable.h"
#include "graphics/texture.h"
#include "data/consume_data_provider.h"
#include "tile_set.h"

namespace projectfarm::graphics
{
    class TileMap final : public ConsumeRenderManager,
                          public Renderable,
                          public shared::ConsumeDataProvider,
                          public shared::time::ConsumeTimer
    {
    public:
        TileMap() = default;
        ~TileMap() override = default;

        static constexpr uint16_t EmptyTileIndex { std::numeric_limits<uint16_t>::max() };

        [[nodiscard]] bool Load(const std::filesystem::path& filePath);
        [[nodiscard]] bool LoadDirect(uint8_t numberOfLayers, uint32_t widthInTiles, uint32_t heightInTiles,
                                      uint32_t tileWidth, uint32_t tileHeight);

        [[nodiscard]] bool SetTileSet(uint8_t tileSetId, const std::shared_ptr<TileSet>& tileSet);

        void Shutdown();
        void ClearTileSets() noexcept;

        void Tick();

        void SetRenderBounds(uint32_t fromX, uint32_t fromY,
                             uint32_t width, uint32_t height) noexcept
        {
            this->_renderBoundsX = fromX;
            this->_renderBoundsY = fromY;
            this->_renderBoundsW = width;
            this->_renderBoundsH = height;
        }

        void Render() noexcept override;

        void RenderGroup(uint8_t groupIndex) noexcept;

        [[nodiscard]] bool SetTileIndex(uint8_t tileSetId, uint32_t layer,
                                        uint32_t tileX, uint32_t tileY, uint32_t indexAbsolute);

        [[nodiscard]] bool SetTileIndex(uint32_t layer, uint32_t tileX, uint32_t tileY,
                                        const std::vector<TileMapTileAnimationData>& animationData);

        void SetTileSize(uint32_t tileWidth, uint32_t tileHeight) noexcept
        {
            this->_tileWidth = tileWidth;
            this->_tileHeight = tileHeight;
        }

        const std::shared_ptr<TileMapTile>& GetTile(uint8_t tileIndex) const noexcept
        {
            return this->_tiles[tileIndex];
        }

        uint16_t GetTile(uint8_t layerIndex, uint32_t x, uint32_t y) const noexcept
        {
            return this->_tileLayers[layerIndex]->GetTile(x, y);
        }

        uint8_t GetNumberOfLayers() const noexcept
        {
            return static_cast<uint8_t>(this->_tileLayers.size());
        }

        void SetLayerGroup(uint8_t layerIndex, uint8_t groupIndex) noexcept
        {
            this->_layerGroups[groupIndex].push_back(layerIndex);
        }

    private:
        bool _isLoaded = false;

        std::unordered_map<uint8_t, std::vector<uint8_t>> _layerGroups;

        std::vector<std::shared_ptr<TileMapTileLayer>> _tileLayers;

        uint32_t _widthInTiles {0};
        uint32_t _heightInTiles {0};
        uint32_t _tileWidth {0};
        uint32_t _tileHeight {0};

        uint32_t _renderBoundsX {0};
        uint32_t _renderBoundsY {0};
        uint32_t _renderBoundsW {0};
        uint32_t _renderBoundsH {0};

        std::map<uint32_t, std::shared_ptr<graphics::TileSet>> _tileSets;

        std::vector<std::shared_ptr<TileMapTile>> _tiles;

        std::vector<uint16_t> _animatedTileIndexes;

        [[nodiscard]] bool LoadTileSet(const nlohmann::json& tileSetJson);
        [[nodiscard]] bool LoadTile(const nlohmann::json& tile);

        void RenderLayer(uint8_t layerIndex) noexcept;
    };
}

#endif