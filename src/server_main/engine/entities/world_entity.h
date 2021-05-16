#ifndef PROJECTFARM_WORLD_ENTITY_H
#define PROJECTFARM_WORLD_ENTITY_H

#include <vector>
#include <cstdint>
#include <memory>

#include "entity.h"
#include "entities/entity_types.h"
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

        [[nodiscard]] shared::entities::EntityTypes GetEntityType() const noexcept override
        {
            return shared::entities::EntityTypes::World;
        }

        [[nodiscard]] std::vector<std::byte> GetEntityData() const noexcept override;

    protected:
        void OnTick() noexcept override;

    private:
        std::shared_ptr<world::World> _world;
    };
}

#endif
