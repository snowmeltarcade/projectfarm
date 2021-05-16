#ifndef PROJECTFARM_SERVER_CLIENT_ENTITY_UPDATE_H
#define PROJECTFARM_SERVER_CLIENT_ENTITY_UPDATE_H

#include <string>

#include "networking/packet.h"
#include "networking/packet_types.h"
#include "entities/entity_types.h"

namespace projectfarm::shared::networking::packets
{
    class ServerClientEntityUpdatePacket final : public Packet
    {
    public:
        ServerClientEntityUpdatePacket() = default;
        ~ServerClientEntityUpdatePacket() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ServerClientEntityUpdate;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_entityId) +
                   this->GetSize(this->_playerId) +
                   this->GetSize(this->_worldName) +
                   this->GetSize(this->_timeOfUpdate) +
                   this->GetSize(this->_entityType) +
                   this->GetSize(this->_entityData);
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        [[nodiscard]] uint64_t GetTimeOfUpdate() const noexcept
        {
            return this->_timeOfUpdate;
        }

        void SetEntityId(uint32_t entityId) noexcept
        {
            this->_entityId = entityId;
        }

        [[nodiscard]] uint32_t GetEntityId() const noexcept
        {
            return this->_entityId;
        }

        void SetPlayerId(uint32_t playerId) noexcept
        {
            this->_playerId = playerId;
        }

        [[nodiscard]] uint32_t GetPlayerId() const noexcept
        {
            return this->_playerId;
        }

        void SetWorldName(const std::string& worldName) noexcept
        {
            this->_worldName = worldName;
        }

        [[nodiscard]] const std::string& GetWorldName() const noexcept
        {
            return this->_worldName;
        }

        void SetTimeOfUpdate(uint64_t timeOfUpdate) noexcept
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

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        uint32_t _entityId {0};
        uint32_t _playerId {0};
        uint64_t _timeOfUpdate {0};
        std::string _worldName;
        entities::EntityTypes _entityType {entities::EntityTypes::Unknown};

        std::vector<std::byte> _entityData;
    };
}

#endif
