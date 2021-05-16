#ifndef PROJECTFARM_PACKET_RECEIVER_H
#define PROJECTFARM_PACKET_RECEIVER_H

#include <memory>

#include "logging/consume_logger.h"
#include "networking/packet.h"

namespace projectfarm::shared::networking
{
    class PacketReceiver final : public shared::ConsumeLogger
    {
    public:
        PacketReceiver() = default;
        ~PacketReceiver() override = default;

        [[nodiscard]] std::shared_ptr<Packet> CheckUDPSocket(UDPsocket socket, UDPpacket* udpPacket) const noexcept;

        [[nodiscard]] static std::pair<bool, std::shared_ptr<Packet>> CheckTCPSocket(TCPsocket socket) noexcept;
    };
}

#endif
