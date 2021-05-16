#ifndef PROJECTFARM_ENTITY_H
#define PROJECTFARM_ENTITY_H

#include <cstdint>
#include <atomic>
#include <vector>

#include "time/consume_timer.h"
#include "entities/entity_types.h"

namespace projectfarm::engine::entities
{
    class Entity : public shared::time::ConsumeTimer
    {
    public:
        Entity() = default;
        ~Entity() override = default;

        void Activate() noexcept
        {
            this->_isActivated = true;
        }

        void Deactivate() noexcept
        {
            this->_isActivated = false;
        }

        void Tick() noexcept;

        [[nodiscard]] bool ShouldBroadcastState() const noexcept
        {
            return this->_isActivated && this->_currentBroadcastStateTime >= this->_broadcastStateTime;
        }

        void SetBroadcastStateTime(uint64_t milliseconds) noexcept
        {
            this->_broadcastStateTime = milliseconds * 1000;
        }

        void ResetBroadcastCounter() noexcept
        {
            this->_currentBroadcastStateTime = 0;
        }

        [[nodiscard]] uint32_t GetEntityId() const noexcept
        {
            return this->_entityId;
        }

        void SetEntityId(uint32_t entityId) noexcept
        {
            this->_entityId = entityId;
        }

        [[nodiscard]] virtual shared::entities::EntityTypes GetEntityType() const noexcept = 0;

        [[nodiscard]] virtual std::vector<std::byte> GetEntityData() const noexcept = 0;

        virtual void OnAfterBroadcastState() noexcept
        {
        }

        virtual void SetData(const std::vector<std::byte>&) noexcept
        {
        }

        [[nodiscard]] bool GetForceSendToOwningPlayer() const noexcept
        {
            return this->_forceSendToOwningPlayer;
        }

        void SetForceSendToOwningPlayer(bool forceSendToOwningPlayer) noexcept
        {
            this->_forceSendToOwningPlayer = forceSendToOwningPlayer;
        }

        [[nodiscard]] uint64_t GetLastUpdateTime() const noexcept
        {
            return this->_lastUpdateTime;
        }

        void SetLastUpdateTime(uint64_t lastUpdateTime) noexcept
        {
            this->_lastUpdateTime = lastUpdateTime;
        }

        [[nodiscard]] virtual uint32_t GetPlayerId() const noexcept
        {
            return 0;
        }

        virtual void SetPlayerId(uint32_t) noexcept
        {
        }

        [[nodiscard]] bool IsPlayer() const noexcept
        {
            return this->GetPlayerId() > 0;
        }

    protected:
        static std::atomic_uint32_t GlobalId;

        virtual void OnTick() noexcept = 0;

        uint32_t _entityId = Entity::GlobalId++;

        uint64_t _lastUpdateTime {0};

        uint64_t _broadcastStateTime {0};
        uint64_t _currentBroadcastStateTime {0};

        // when created, ensure this entity broadcasts its state so it is created on the client side
        bool _forceSendToOwningPlayer {true};

        bool _isActivated {true};
    };
}

#endif
