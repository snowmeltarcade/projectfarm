#ifndef PROJECTFARM_SERVER_CLIENT_CHATBOX_MESSAGE_H
#define PROJECTFARM_SERVER_CLIENT_CHATBOX_MESSAGE_H

#include <string>

#include "../packet.h"
#include "../packet_types.h"

namespace projectfarm::shared::networking::packets
{
    class ServerClientChatboxMessagePacket final : public Packet
    {
    public:
        ServerClientChatboxMessagePacket() = default;
        ~ServerClientChatboxMessagePacket() override = default;

        [[nodiscard]]
        PacketTypes GetPacketType() const override
        {
            return PacketTypes::ServerClientChatboxMessage;
        }

        [[nodiscard]]
        uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_username) +
                   this->GetSize(this->_message) +
                   this->GetSize(this->_serverTime);
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        [[nodiscard]]
        bool IsVital() const override
        {
            return true;
        }

        [[nodiscard]]
        const std::string& GetUsername() const noexcept
        {
            return this->_username;
        }

        void SetUsername(const std::string& username) noexcept
        {
            this->_username = username;
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

        [[nodiscard]]
        uint64_t GetServerTime() const noexcept
        {
            return this->_serverTime;
        }

        void SetServerTime(uint64_t serverTime) noexcept
        {
            this->_serverTime = serverTime;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        std::string _username;
        std::string _message;
        uint64_t _serverTime {0};
    };
}

#endif
