#ifndef PROJECTFARM_SERVER_CLIENT_SET_PLAYER_DETAILS_H
#define PROJECTFARM_SERVER_CLIENT_SET_PLAYER_DETAILS_H

#include <string>

#include "../packet.h"
#include "../packet_types.h"

namespace projectfarm::shared::networking::packets
{
    class ServerClientSetPlayerDetails final : public Packet
    {
    public:
        ServerClientSetPlayerDetails() = default;
        ~ServerClientSetPlayerDetails() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ServerClientSetPlayerDetails;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_playerId);
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

        [[nodiscard]] bool IsVital() const override
        {
            return true;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        uint32_t _playerId {0};
    };
}

#endif
