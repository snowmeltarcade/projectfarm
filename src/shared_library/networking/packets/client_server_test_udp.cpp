#include <array>

#include "utils/util.h"
#include "client_server_test_udp.h"

namespace projectfarm::shared::networking::packets
{
    void ClientServerTestUdp::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        pfu::WriteUInt32(bytes, this->_playerId);
    }

    void ClientServerTestUdp::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_playerId = pfu::ReadUInt32(bytes, index);
    }

    void ClientServerTestUdp::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "Player Id", this->_playerId);
    }
}