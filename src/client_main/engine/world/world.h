#ifndef PROJECTFARM_WORLD_H
#define PROJECTFARM_WORLD_H

#include <string>
#include <filesystem>
#include <memory>
#include <vector>
#include <list>
#include <optional>

#include "time/consume_timer.h"
#include "logging/consume_logger.h"
#include "data/consume_data_provider.h"
#include "graphics/consume_graphics.h"
#include "graphics/consume_render_manager.h"
#include "graphics/consume_tile_set_pool.h"
#include "engine/consume_game.h"
#include "graphics/renderable.h"
#include "island.h"
#include "plots.h"
#include "engine/entities/entity.h"
#include "entities/entity_types.h"
#include "scenes/consume_scene.h"

namespace projectfarm::engine::entities
{
    class Character;
}

namespace projectfarm::engine::world
{
    class World final : public shared::ConsumeDataProvider,
                        public graphics::ConsumeRenderManager,
                        public graphics::ConsumeTileSetPool,
                        public graphics::Renderable,
                        public shared::time::ConsumeTimer,
                        public engine::ConsumeGame,
                        public scenes::ConsumeScene,
                        public std::enable_shared_from_this<World>
    {
    public:
        World()
        {
            this->_plots = std::make_shared<Plots>();
        }
        ~World() override = default;

        [[nodiscard]] bool Load(const std::string& name);
        [[nodiscard]] bool Load(const std::filesystem::path& filePath);

        void Shutdown();

        [[nodiscard]] std::string GetName() const
        {
            return this->_name;
        }

        void Tick();

        void Render() noexcept override;

        [[nodiscard]] bool UpdateEntity(uint32_t entityId, uint32_t playerId, shared::entities::EntityTypes entityType,
                                        uint64_t lastUpdateTime, const std::vector<std::byte>& data) noexcept;

        [[nodiscard]] const std::vector<std::shared_ptr<Island>>& GetIslands() const noexcept
        {
            return this->_islands;
        }

        [[nodiscard]] const std::shared_ptr<Plots>& GetPlots() const noexcept
        {
            return this->_plots;
        }

        void RemoveEntity(uint32_t entityId) noexcept;

        void ReconfirmPixelSizes() noexcept override;

        [[nodiscard]] uint16_t GetPlotIndexFromWorldPosition(float x, float y) const noexcept;
        [[nodiscard]] std::pair<int32_t, int32_t> GetTileIndexesFromWorldPosition(float x, float y) const noexcept;

        [[nodiscard]] std::optional<std::shared_ptr<entities::Character>> GetCharacterByEntityId(uint32_t entityId) const noexcept;

    private:
        std::string _name;

        std::shared_ptr<Plots> _plots;
        std::vector<std::shared_ptr<Island>> _islands;
        std::shared_ptr<Island> _backgroundIsland;

        std::list<std::shared_ptr<entities::Entity>> _entities;

        [[nodiscard]] bool LoadFromJsonFile(const std::filesystem::path& filePath);
        [[nodiscard]] bool LoadFromBinaryFile(const std::filesystem::path& filePath);

        [[nodiscard]] bool LoadBackground(const nlohmann::json& backgroundJson);
        [[nodiscard]] bool LoadBackgroundIsland(const nlohmann::json& islandJson);

        void UpdateEntities();

        void UpdateIslandRenderBounds() noexcept;

        [[nodiscard]] std::shared_ptr<entities::Entity> CreateNewEntity(uint32_t entityId, uint32_t playerId,
                shared::entities::EntityTypes entityType) noexcept;
    };
}

#endif
