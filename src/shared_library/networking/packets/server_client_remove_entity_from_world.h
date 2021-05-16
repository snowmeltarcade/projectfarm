#ifndef PROJECTFARM_SERVER_CLIENT_REMOVE_ENTITY_FROM_WORLD_H
#define PROJECTFARM_SERVER_CLIENT_REMOVE_ENTITY_FROM_WORLD_H

#include <string>

#include "../packet.h"
#include "../packet_types.h"

namespace projectfarm::shared::networking::packets
{
    class ServerClientRemoveEntityFromWorld final : public Packet
    {
    public:
        ServerClientRemoveEntityFromWorld() = default;
        ~ServerClientRemoveEntityFromWorld() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ServerClientRemoveEntityFromWorld;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_entityId) +
                   this->GetSize(this->_worldName);
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        [[nodiscard]] uint32_t GetEntityId() const noexcept
        {
            return this->_entityId;
        }

        void SetEntityId(uint32_t entityId) noexcept
        {
            this->_entityId = entityId;
        }

        [[nodiscard]] const std::string& GetWorldName() const noexcept
        {
            return this->_worldName;
        }

        void SetWorldName(const std::string& worldName) noexcept
        {
            this->_worldName = worldName;
        }

        [[nodiscard]] bool IsVital() const override
        {
            return true;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        uint32_t _entityId {0};

        std::string _worldName;
    };
}

#endif
