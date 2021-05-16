#ifndef PROJECTFARM_CLIENT_SERVER_REQUEST_HASHED_PASSWORD_H
#define PROJECTFARM_CLIENT_SERVER_REQUEST_HASHED_PASSWORD_H

#include <string>

#include "../packet.h"
#include "../packet_types.h"

namespace projectfarm::shared::networking::packets
{
    class ClientServerRequestHashedPasswordPacket final : public Packet
    {
    public:
        ClientServerRequestHashedPasswordPacket() = default;
        ~ClientServerRequestHashedPasswordPacket() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ClientServerRequestHashedPassword;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_userName);
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        [[nodiscard]] bool IsVital() const override
        {
            return true;
        }

        [[nodiscard]] const std::string& GetUserName() const noexcept
        {
            return this->_userName;
        }

        void SetUserName(const std::string& userName) noexcept
        {
            this->_userName = userName;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        std::string _userName;
    };
}

#endif
