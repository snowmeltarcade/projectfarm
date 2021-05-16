#ifndef PROJECTFARM_SERVER_CLIENT_LOAD_WORLD_H
#define PROJECTFARM_SERVER_CLIENT_LOAD_WORLD_H

#include <string>

#include "../packet.h"
#include "../packet_types.h"

namespace projectfarm::shared::networking::packets
{
    class ServerClientLoadWorldPacket final : public Packet
    {
    public:
        ServerClientLoadWorldPacket() = default;
        ~ServerClientLoadWorldPacket() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ServerClientLoadWorld;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_worldToLoad);
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        [[nodiscard]] const std::string& GetWorldToLoad() const noexcept
        {
            return this->_worldToLoad;
        }

        void SetWorldToLoad(const std::string& worldToLoad) noexcept
        {
            this->_worldToLoad = worldToLoad;
        }

        [[nodiscard]] bool IsVital() const override
        {
            return true;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        std::string _worldToLoad;
    };
}

#endif
