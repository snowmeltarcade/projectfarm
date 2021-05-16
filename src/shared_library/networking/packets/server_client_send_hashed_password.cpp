#include <array>

#include "utils/util.h"
#include "server_client_send_hashed_password.h"

namespace projectfarm::shared::networking::packets
{
    void ServerClientSendHashedPasswordPacket::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        pfu::WriteString(bytes, this->_userName, static_cast<uint32_t>(this->_userName.size()));
        pfu::WriteString(bytes, this->_hashedPassword, static_cast<uint32_t>(this->_hashedPassword.size()));
    }

    void ServerClientSendHashedPasswordPacket::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_userName = pfu::ReadString(bytes, index);
        this->_hashedPassword = pfu::ReadString(bytes, index);
    }

    void ServerClientSendHashedPasswordPacket::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "User Name", this->_userName);
        this->SerializeDebugData(ss, "Hashed Password", this->_hashedPassword);
    }
}