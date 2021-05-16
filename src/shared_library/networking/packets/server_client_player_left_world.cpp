

#include "utils/util.h"
#include "server_client_player_left_world.h"

namespace projectfarm::shared::networking::packets
{
    void ServerClientPlayerLeftWorld::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        pfu::WriteUInt32(bytes, this->_playerId);
        pfu::WriteString(bytes, this->_worldName, static_cast<uint32_t>(this->_worldName.size()));
    }

    void ServerClientPlayerLeftWorld::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_playerId = pfu::ReadUInt32(bytes, index);
        this->_worldName = pfu::ReadString(bytes, index);
    }

    void ServerClientPlayerLeftWorld::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "Player Id", this->_playerId);
        this->SerializeDebugData(ss, "World Name", this->_worldName);
    }
}