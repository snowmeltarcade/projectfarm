#include <array>

#include "utils/util.h"
#include "client_server_request_hashed_password.h"

namespace projectfarm::shared::networking::packets
{
    void ClientServerRequestHashedPasswordPacket::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        pfu::WriteString(bytes, this->_userName, static_cast<uint32_t>(this->_userName.size()));
    }

    void ClientServerRequestHashedPasswordPacket::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_userName = pfu::ReadString(bytes, index);
    }

    void ClientServerRequestHashedPasswordPacket::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "User Name", this->_userName);
    }
}