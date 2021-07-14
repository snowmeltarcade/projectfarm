#ifndef PROJECTFARM_ISLAND_H
#define PROJECTFARM_ISLAND_H

#include <cstdint>
#include <memory>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <nlohmann/json.hpp>

#include "time/consume_timer.h"
#include "graphics/tile_map.h"
#include "data/consume_data_provider.h"
#include "graphics/consume_graphics.h"
#include "graphics/consume_render_manager.h"
#include "graphics/consume_tile_set_pool.h"
#include "graphics/renderable.h"
#include "plots.h"
#include "graphics/graphics.h"

namespace projectfarm::engine::world
{
    class Island final : public shared::ConsumeDataProvider,
                         public graphics::ConsumeRenderManager,
                         public graphics::ConsumeTileSetPool,
                         public graphics::Renderable,
                         public shared::time::ConsumeTimer
    {
    public:
        using LayerType = std::vector<std::vector<uint16_t>>;
        using LayersType = std::vector<std::pair<bool, LayerType>>;

        Island() = default;
        ~Island() override = default;

        [[nodiscard]] bool LoadFromJson(const nlohmann::json& jsonFile,
                                        const std::shared_ptr<Plots>& plots);
        [[nodiscard]] bool LoadFromBinary(std::ifstream& fs,
                                          const std::shared_ptr<Plots>& plots) noexcept;

        void Shutdown();

        void Tick();

        void Render() noexcept override;
        void RenderGround() noexcept;
        void RenderOverhead() noexcept;

        [[nodiscard]] uint32_t GetWidthInPixels() const noexcept
        {
            return this->GetGraphics()->MetersToPixels(this->_tileWidth) * this->_widthInTiles;
        }

        [[nodiscard]] uint32_t GetHeightInPixels() const noexcept
        {
            return this->GetGraphics()->MetersToPixels(this->_tileHeight) * this->_heightInTiles;
        }

        [[nodiscard]] uint32_t GetTileWidthInPixels() const noexcept
        {
            return static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_tileWidth));
        }

        [[nodiscard]] uint32_t GetHeightInTiles() const noexcept
        {
            return this->_heightInTiles;
        }

        void ReconfirmPixelSizes() noexcept override;

        void SetRenderBounds(uint32_t tileX, uint32_t tileY,
                             uint32_t widthInTiles, uint32_t heightInTiles) noexcept
        {
            this->_tileMap->SetRenderBounds(tileX, tileY, widthInTiles, heightInTiles);
        }

        void SetRenderBounds(const SDL_Rect& viewport) noexcept;

        [[nodiscard]] uint64_t GetLastPlotUpdateTime() const noexcept
        {
            return this->_lastPlotUpdateTime;
        }

        void ProcessPlotUpdate(uint8_t layerIndex, uint32_t tileX, uint32_t tileY,
                               uint64_t updateTime, uint16_t plotIndex) noexcept;

        [[nodiscard]] uint16_t GetPlotIndexAtWorldPosition(float x, float y) const noexcept;
        [[nodiscard]] std::pair<int32_t, int32_t> GetTileIndexesFromWorldPosition(float x, float y) const noexcept;

    private:
        uint32_t _widthInTiles {0};
        uint32_t _heightInTiles {0};

        float _widthInMeters {0.0f};
        float _heightInMeters {0.0f};

        float _tileWidth {0.0f};
        float _tileHeight {0.0f};

        float _positionX {0.0f};
        float _positionY {0.0f};

        std::shared_ptr<Plots> _plots;

        std::shared_ptr<graphics::TileMap> _tileMap;
        std::unordered_map<uint16_t, uint16_t> _tileMapPlotMap;

        void RenderRegionToLayer(uint16_t plotIndex,
                                 uint32_t x, uint32_t y,
                                 uint32_t w, uint32_t h,
                                 LayerType& layer) const noexcept;

        [[nodiscard]] LayersType BuildLayers(const nlohmann::json& layersJson);
        [[nodiscard]] LayersType BuildLayers(std::ifstream& fs) const noexcept;

        [[nodiscard]] LayerType CreateLayer(const std::string& plotName) const noexcept;
        [[nodiscard]] LayerType CreateLayer(std::ifstream& fs) const noexcept;

        std::unordered_map<std::string, uint32_t> _tileMapIndexes;

        bool IndexTileSets();

        bool RefreshTileSet(const LayersType& layers);

        [[nodiscard]] auto UpdatePlotOnTileMap(uint16_t plotIndex,
                                               uint8_t layer, uint32_t x, uint32_t y) noexcept -> bool;

        [[nodiscard]] auto GetAnimationInfoFromPlot(const std::shared_ptr<Plot>& plot) noexcept
            -> std::vector<graphics::TileMapTileAnimationData>;

        uint64_t _lastPlotUpdateTime {0};
    };
}

#endif
