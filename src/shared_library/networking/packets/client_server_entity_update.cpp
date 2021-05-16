#include <algorithm>
#include <vector>

#include "utils/util.h"
#include "client_server_entity_update.h"

namespace projectfarm::shared::networking::packets
{
    void ClientServerEntityUpdatePacket::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        auto entityDataSize = static_cast<uint32_t>(this->_entityData.size());

        pfu::WriteUInt32(bytes, this->_playerId);
        pfu::WriteUInt32(bytes, this->_entityId);
        pfu::WriteUInt64(bytes, this->_timeOfUpdate);
        pfu::WriteUInt8(bytes, static_cast<uint8_t>(this->_entityType));
        pfu::WriteUInt32(bytes, entityDataSize);

        if (entityDataSize > 0)
        {
            std::copy(this->_entityData.begin(), this->_entityData.end(),
                        std::back_inserter(bytes));
        }
    }

    void ClientServerEntityUpdatePacket::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_playerId = pfu::ReadUInt32(bytes, index);
        this->_entityId = pfu::ReadUInt32(bytes, index);
        this->_timeOfUpdate = pfu::ReadUInt64(bytes, index);
        this->_entityType = static_cast<entities::EntityTypes>(pfu::ReadUInt8(bytes, index));

        auto dataSize = pfu::ReadUInt32(bytes, index);

        if (dataSize > 0)
        {
            this->_entityData.reserve(dataSize);

            std::copy(bytes.begin() + index, bytes.begin() + index + dataSize,
                        std::back_inserter(this->_entityData));

            index += dataSize;
        }
    }

    void ClientServerEntityUpdatePacket::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "Player Id", this->_playerId);
        this->SerializeDebugData(ss, "Entity Id", this->_entityId);
        this->SerializeDebugData(ss, "Time of Update", this->_timeOfUpdate);
        this->SerializeDebugData(ss, "Entity Type", static_cast<int>(this->_entityType));
        this->SerializeDebugData(ss, "Data Size", this->_entityData.size());

        ss << "Data: ";
        std::for_each(this->_entityData.begin(), this->_entityData.end(), [&](const auto& d)
        {
           ss << std::to_string(static_cast<uint8_t>(d)) << ",";
        });
    }
}