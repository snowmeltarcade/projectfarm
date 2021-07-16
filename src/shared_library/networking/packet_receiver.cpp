#include <cstdint>
#include <SDL_net.h>

#include "packet_receiver.h"
#include "networking/packet_factory.h"
#include "api/logging/logging.h"

namespace projectfarm::shared::networking
{
    std::shared_ptr<Packet> PacketReceiver::CheckUDPSocket(UDPsocket socket, UDPpacket* udpPacket) const noexcept
    {
        if (SDLNet_UDP_Recv(socket, udpPacket) <= 0)
        {
            return nullptr;
        }

        if (udpPacket->len <= 0)
        {
            api::logging::Log("Did not receive packet data.");
            return {};
        }

        auto data = reinterpret_cast<std::byte*>(udpPacket->data);

        int startOffset = sizeof(std::byte) * 4; // the first 4 bytes are the packet's size, so the 5th (index 4) byte is what we care about

        std::byte packetTypeNumber {data[startOffset]};

        const auto packetType { static_cast<projectfarm::shared::networking::PacketTypes>(packetTypeNumber) };
        auto packet { projectfarm::shared::networking::PacketFactory::CreatePacket(packetType) };

        startOffset += sizeof(std::byte); // skip the byte we just read

        // start from the byte after the packet type
        std::vector<std::byte> buff;
        buff.reserve(udpPacket->len - startOffset);
        for (int i = startOffset; i < udpPacket->len; ++i)
        {
            buff.push_back(data[i]);
        }

        packet->FromBytes(buff);

        return packet;
    }

    std::pair<bool, std::shared_ptr<Packet>> PacketReceiver::CheckTCPSocket(TCPsocket socket) noexcept
    {
        if (SDLNet_SocketReady(socket))
        {
            uint8_t size[4];

            int amountReceived = SDLNet_TCP_Recv(socket, size, sizeof(uint8_t) * 4);
            if (amountReceived <= 0)
            {
                return {false, nullptr};
            }

            const uint32_t packetSize = (size[0ul] << 24ul) |
                                        (size[1ul] << 16ul) |
                                        (size[2ul] << 8ul) |
                                        (size[3ul] << 0ul);

            uint8_t packetTypeNumber {0};

            amountReceived = SDLNet_TCP_Recv(socket, &packetTypeNumber, sizeof(packetTypeNumber));
            if (amountReceived <= 0)
            {
                return {false, nullptr};
            }

            auto packetType { static_cast<projectfarm::shared::networking::PacketTypes>(packetTypeNumber) };
            auto packet { projectfarm::shared::networking::PacketFactory::CreatePacket(packetType) };

            auto sizeToRead { packetSize - sizeof(packetSize) - sizeof(packetTypeNumber) };

            if (sizeToRead > 0)
            {
                auto buff = std::vector<std::byte>();
                buff.reserve(sizeToRead);
                const auto buffData = static_cast<void*>(&buff.front());

                amountReceived = SDLNet_TCP_Recv(socket, buffData, static_cast<int>(sizeof(uint8_t) * sizeToRead));
                if (amountReceived <= 0)
                {
                    return {false, nullptr};
                }

                packet->FromBytes(buff);
            }

#ifdef LOGGING_PACKET_DEBUG_INFO
            //auto debugData = packet->GetDebugData();
            //api::logging::Log(debugData);
#endif

            return {true, packet};
        }

        return {true, nullptr};
    }
}