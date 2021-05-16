#include <array>

#include "utils/util.h"
#include "server_client_remove_entity_from_world.h"

namespace projectfarm::shared::networking::packets
{
    void ServerClientRemoveEntityFromWorld::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        pfu::WriteUInt32(bytes, this->_entityId);
        pfu::WriteString(bytes, this->_worldName, static_cast<uint32_t>(this->_worldName.size()));
    }

    void ServerClientRemoveEntityFromWorld::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_entityId = pfu::ReadUInt32(bytes, index);
        this->_worldName = pfu::ReadString(bytes, index);
    }

    void ServerClientRemoveEntityFromWorld::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "Entity Id", this->_entityId);
        this->SerializeDebugData(ss, "World Name", this->_worldName);
    }
}