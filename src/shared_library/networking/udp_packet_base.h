#ifndef PROJECTFARM_UDP_PACKET_BASE_H
#define PROJECTFARM_UDP_PACKET_BASE_H

#include <cstdint>

#include "packet.h"

namespace projectfarm::shared::networking
{
    class UDPPacketBase : public Packet
    {
    public:
        [[nodiscard]] virtual uint32_t GetPlayerId() const noexcept = 0;
    };
}

#endif
