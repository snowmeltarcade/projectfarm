#include <array>

#include "utils/util.h"
#include "server_client_load_world.h"

namespace projectfarm::shared::networking::packets
{
    void ServerClientLoadWorldPacket::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        pfu::WriteString(bytes, this->_worldToLoad, static_cast<uint32_t>(this->_worldToLoad.size()));
    }

    void ServerClientLoadWorldPacket::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_worldToLoad = pfu::ReadString(bytes, index);
    }

    void ServerClientLoadWorldPacket::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "World to Load", this->_worldToLoad);
    }
}