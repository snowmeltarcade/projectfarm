#include "world_entity.h"

namespace projectfarm::engine::entities
{
    void WorldEntity::OnTick() noexcept
    {
    }

    std::vector<std::byte> WorldEntity::GetEntityData() const noexcept
    {
        return this->_world->GetDataForClientSerialization();
    }
}