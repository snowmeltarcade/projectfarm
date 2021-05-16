#ifndef PROJECTFARM_CLIENT_SERVER_ENTITY_UPDATE_H
#define PROJECTFARM_CLIENT_SERVER_ENTITY_UPDATE_H

#include <string>

#include "networking/udp_packet_base.h"
#include "networking/packet_types.h"
#include "entities/entity_types.h"

namespace projectfarm::shared::networking::packets
{
    class ClientServerEntityUpdatePacket final : public UDPPacketBase
    {
    public:
        ClientServerEntityUpdatePacket() = default;
        ~ClientServerEntityUpdatePacket() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ClientServerEntityUpdate;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_playerId) +
                   this->GetSize(this->_entityId) +
                   this->GetSize(this->_timeOfUpdate) +
                   this->GetSize(this->_entityType) +
                   this->GetSize(this->_entityData);
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        [[nodiscard]] uint64_t GetTimeOfUpdate() const
        {
            return this->_timeOfUpdate;
        }

        void SetEntityId(uint32_t entityId)
        {
            this->_entityId = entityId;
        }

        [[nodiscard]] uint32_t GetEntityId() const
        {
            return this->_entityId;
        }

        void SetTimeOfUpdate(uint64_t timeOfUpdate)
        {
            this->_timeOfUpdate = timeOfUpdate;
        }

        [[nodiscard]] entities::EntityTypes GetEntityType() const noexcept
        {
            return this->_entityType;
        }

        void SetEntityType(entities::EntityTypes entityType) noexcept
        {
            this->_entityType = entityType;
        }

        [[nodiscard]] std::vector<std::byte> GetEntityData() const noexcept
        {
            return this->_entityData;
        }

        // this method must be set *before* SizeInBytes() is called in
        // order to get an accurate packet size
        void SetEntityData(const std::vector<std::byte>& data) noexcept
        {
            this->_entityData = data;
        }

        [[nodiscard]] bool IsVital() const override
        {
            return false;
        }

        [[nodiscard]] uint32_t GetPlayerId() const noexcept override
        {
            return this->_playerId;
        }

        void SetPlayerId(uint32_t playerId) noexcept
        {
            this->_playerId = playerId;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        uint32_t _playerId {0};
        uint32_t _entityId {0};
        uint64_t _timeOfUpdate {0};
        entities::EntityTypes _entityType = entities::EntityTypes::Unknown;

        std::vector<std::byte> _entityData;
    };
}

#endif
