#include "entity.h"
#include "time/timer.h"

namespace projectfarm::engine::entities
{
    void Entity::Tick() noexcept
    {
        this->_currentBroadcastStateTime += this->GetTimer()->GetLastFrameDurationInMicroseconds();

        this->OnTick();
    }
}
