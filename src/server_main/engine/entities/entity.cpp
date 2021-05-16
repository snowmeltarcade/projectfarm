#include "engine/entities/entity.h"

#include "time/timer.h"

namespace projectfarm::engine::entities
{
    std::atomic_uint32_t Entity::GlobalId {0u};

    void Entity::Tick() noexcept
    {
        this->_currentBroadcastStateTime += this->GetTimer()->GetLastFrameDurationInMicroseconds();

        this->OnTick();
    }
}