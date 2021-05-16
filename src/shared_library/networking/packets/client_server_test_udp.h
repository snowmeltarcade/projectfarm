#ifndef PROJECTFARM_CLIENT_SERVER_TEST_UDP_H
#define PROJECTFARM_CLIENT_SERVER_TEST_UDP_H

#include <string>

#include "../udp_packet_base.h"
#include "../packet_types.h"

namespace projectfarm::shared::networking::packets
{
    class ClientServerTestUdp final : public UDPPacketBase
    {
    public:
        ClientServerTestUdp() = default;
        ~ClientServerTestUdp() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ClientServerTestUdp;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_playerId);
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        [[nodiscard]] uint32_t GetPlayerId() const noexcept override
        {
            return this->_playerId;
        }

        void SetPlayerId(uint32_t playerId) noexcept
        {
            this->_playerId = playerId;
        }

        [[nodiscard]] bool IsVital() const override
        {
            return false;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        uint32_t _playerId {0};
    };
}

#endif
