#include <array>

#include "utils/util.h"
#include "client_server_chatbox_message.h"

namespace projectfarm::shared::networking::packets
{
    void ClientServerChatboxMessagePacket::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        pfu::WriteString(bytes, this->_message, static_cast<uint32_t>(this->_message.size()));
    }

    void ClientServerChatboxMessagePacket::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_message = pfu::ReadString(bytes, index);
    }

    void ClientServerChatboxMessagePacket::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "Message", this->_message);
    }
}