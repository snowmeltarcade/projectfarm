#include <array>

#include "utils/util.h"
#include "server_client_chatbox_message.h"

namespace projectfarm::shared::networking::packets
{
    void ServerClientChatboxMessagePacket::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        pfu::WriteString(bytes, this->_username, static_cast<uint32_t>(this->_username.size()));
        pfu::WriteString(bytes, this->_message, static_cast<uint32_t>(this->_message.size()));
        pfu::WriteUInt64(bytes, this->_serverTime);
    }

    void ServerClientChatboxMessagePacket::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_username = pfu::ReadString(bytes, index);
        this->_message = pfu::ReadString(bytes, index);
        this->_serverTime = pfu::ReadUInt64(bytes, index);
    }

    void ServerClientChatboxMessagePacket::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "Username", this->_username);
        this->SerializeDebugData(ss, "Message", this->_message);
        this->SerializeDebugData(ss, "Server Time", this->_serverTime);
    }
}