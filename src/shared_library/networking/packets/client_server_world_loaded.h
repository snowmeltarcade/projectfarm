#ifndef PROJECTFARM_CLIENT_SERVER_WORLD_LOADED_H
#define PROJECTFARM_CLIENT_SERVER_WORLD_LOADED_H

#include <string>

#include "../packet.h"
#include "../packet_types.h"

namespace projectfarm::shared::networking::packets
{
    class ClientServerWorldLoaded final : public Packet
    {
    public:
        ClientServerWorldLoaded() = default;
        ~ClientServerWorldLoaded() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ClientServerWorldLoaded;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return 0;
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        [[nodiscard]] bool IsVital() const override
        {
            return true;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;
    };
}

#endif
