#ifndef PROJECTFARM_CLIENT_SERVER_CHATBOX_MESSAGE_H
#define PROJECTFARM_CLIENT_SERVER_CHATBOX_MESSAGE_H

#include <string>

#include "../packet.h"
#include "../packet_types.h"

namespace projectfarm::shared::networking::packets
{
    class ClientServerChatboxMessagePacket final : public Packet
    {
    public:
        ClientServerChatboxMessagePacket() = default;
        ~ClientServerChatboxMessagePacket() override = default;

        [[nodiscard]]
        PacketTypes GetPacketType() const override
        {
            return PacketTypes::ClientServerChatboxMessage;
        }

        [[nodiscard]]
        uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_message);
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        [[nodiscard]]
        bool IsVital() const override
        {
            return true;
        }

        [[nodiscard]]
        const std::string& GetMessage() const noexcept
        {
            return this->_message;
        }

        void SetMessage(const std::string& message) noexcept
        {
            this->_message = message;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        std::string _message;
    };
}

#endif
