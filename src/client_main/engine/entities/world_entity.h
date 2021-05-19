#ifndef PROJECTFARM_WORLD_ENTITY_H
#define PROJECTFARM_WORLD_ENTITY_H

#include <vector>
#include <cstdint>
#include <memory>

#include "entity.h"
#include "engine/world/world.h"

namespace projectfarm::engine::entities
{
    class WorldEntity final : public Entity
    {
    public:
        explicit WorldEntity(std::shared_ptr<world::World> world)
            : _world {std::move(world)}
        {}
        ~WorldEntity() override = default;

        void Render() noexcept override;

        [[nodiscard]] bool Load() noexcept override
        {
            return true;
        }

        void Shutdown() noexcept override
        {
        }

        shared::entities::EntityTypes GetEntityType() const noexcept override
        {
            return shared::entities::EntityTypes::World;
        }

        void SetData(const std::vector<std::byte>& data) noexcept override;

    protected:
        void OnTick() noexcept override;

    private:
        std::shared_ptr<world::World> _world;
    };
}

#endif
