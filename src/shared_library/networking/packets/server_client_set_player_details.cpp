#include <array>

#include "utils/util.h"
#include "server_client_set_player_details.h"

namespace projectfarm::shared::networking::packets
{
    void ServerClientSetPlayerDetails::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        pfu::WriteUInt32(bytes, this->_playerId);
    }

    void ServerClientSetPlayerDetails::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_playerId = pfu::ReadUInt32(bytes, index);
    }

    void ServerClientSetPlayerDetails::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "Player Id", this->_playerId);
    }
}