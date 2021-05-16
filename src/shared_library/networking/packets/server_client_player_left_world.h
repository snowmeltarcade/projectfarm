#ifndef PROJECTFARM_SERVER_CLIENT_PLAYER_LEFT_WORLD_H
#define PROJECTFARM_SERVER_CLIENT_PLAYER_LEFT_WORLD_H

#include <string>

#include "../packet.h"
#include "../packet_types.h"

namespace projectfarm::shared::networking::packets
{
    class ServerClientPlayerLeftWorld final : public Packet
    {
    public:
        ServerClientPlayerLeftWorld() = default;
        ~ServerClientPlayerLeftWorld() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ServerClientPlayerLeftWorld;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_playerId) +
                   this->GetSize(this->_worldName);
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        [[nodiscard]] uint32_t GetPlayerId() const noexcept
        {
            return this->_playerId;
        }

        void SetPlayerId(uint32_t playerId) noexcept
        {
            this->_playerId = playerId;
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
        uint32_t _playerId {0};

        std::string _worldName;
    };
}

#endif
