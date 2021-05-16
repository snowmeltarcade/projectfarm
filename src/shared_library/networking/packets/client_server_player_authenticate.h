#ifndef PROJECTFARM_CLIENT_SERVER_PLAYER_AUTHENTICATE_H
#define PROJECTFARM_CLIENT_SERVER_PLAYER_AUTHENTICATE_H

#include <string>

#include "../packet.h"
#include "../packet_types.h"

namespace projectfarm::shared::networking::packets
{
    class ClientServerPlayerAuthenticatePacket final : public Packet
    {
    public:
        ClientServerPlayerAuthenticatePacket() = default;
        ~ClientServerPlayerAuthenticatePacket() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ClientServerPlayerAuthenticate;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_userName) +
                   this->GetSize(this->_hashedPassword);
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

        [[nodiscard]] const std::string& GetHashedPassword() const noexcept
        {
            return this->_hashedPassword;
        }

        void SetHashedPassword(const std::string& hashedPassword) noexcept
        {
            this->_hashedPassword = hashedPassword;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        std::string _userName;
        std::string _hashedPassword;
    };
}

#endif
