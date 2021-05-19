#ifndef PROJECTFARM_ENTITY_H
#define PROJECTFARM_ENTITY_H

#include <cstdint>
#include <vector>

#include "entities/entity_types.h"
#include "graphics/renderable.h"
#include "time/consume_timer.h"

namespace projectfarm::engine::entities
{
    class Entity : public graphics::Renderable,
                   public shared::time::ConsumeTimer
    {
    public:
        Entity() = default;
        ~Entity() override = default;

        void Tick() noexcept;

        [[nodiscard]] virtual bool Load() noexcept = 0;
        virtual void Shutdown() noexcept = 0;

        virtual shared::entities::EntityTypes GetEntityType() const noexcept = 0;

        [[nodiscard]] uint32_t GetEntityId() const noexcept
        {
            return this->_entityId;
        }

        void SetEntityId(uint32_t entityId) noexcept
        {
            this->_entityId = entityId;
        }

        [[nodiscard]] uint64_t GetLastUpdateTime() const noexcept
        {
            return this->_lastUpdateTime;
        }

        void SetLastUpdateTime(uint64_t lastUpdateTime) noexcept
        {
            this->_lastUpdateTime = lastUpdateTime;
        }

        virtual void SetData(const std::vector<std::byte>&) noexcept
        {
        }

        [[nodiscard]] virtual std::vector<std::byte> GetEntityData() const noexcept
        {
            return {};
        }

        [[nodiscard]] bool ShouldBroadcastState() const noexcept
        {
            return this->_shouldBroadcastState &&
                   this->_currentBroadcastStateTime >= this->_broadcastStateTime;
        }

        void SetBroadcastStateTime(uint64_t milliseconds) noexcept
        {
            this->_broadcastStateTime = milliseconds * 1000;
        }

        void ResetBroadcastCounter() noexcept
        {
            this->_currentBroadcastStateTime = 0;
        }

        void SetShouldBroadcastState(bool shouldBroadcastState) noexcept
        {
            this->_shouldBroadcastState = shouldBroadcastState;
        }

    protected:
        virtual void OnTick() noexcept = 0;

        uint32_t _entityId {0};
        uint64_t _lastUpdateTime {0};

        bool _shouldBroadcastState {false};
        uint64_t _broadcastStateTime {0};
        uint64_t _currentBroadcastStateTime {0};
    };
}

#endif
