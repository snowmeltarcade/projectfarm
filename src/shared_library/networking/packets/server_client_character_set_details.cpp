#include <vector>

#include "utils/util.h"
#include "server_client_character_set_details.h"

namespace projectfarm::shared::networking::packets
{
    void ServerClientCharacterSetDetailsPacket::SerializeBytes(std::vector<std::byte>& bytes) const noexcept
    {
        pfu::WriteUInt32(bytes, this->_entityId);
        pfu::WriteString(bytes, this->_worldName, static_cast<uint32_t>(this->_worldName.size()));
        pfu::WriteString(bytes, this->_appearanceDetails.Hair, static_cast<uint32_t>(this->_appearanceDetails.Hair.size()));
        pfu::WriteString(bytes, this->_appearanceDetails.Body, static_cast<uint32_t>(this->_appearanceDetails.Body.size()));
        pfu::WriteString(bytes, this->_appearanceDetails.ClothesTop, static_cast<uint32_t>(this->_appearanceDetails.ClothesTop.size()));
        pfu::WriteString(bytes, this->_appearanceDetails.ClothesBottom, static_cast<uint32_t>(this->_appearanceDetails.ClothesBottom.size()));
        pfu::WriteString(bytes, this->_appearanceDetails.Feet, static_cast<uint32_t>(this->_appearanceDetails.Feet.size()));
    }

    void ServerClientCharacterSetDetailsPacket::FromBytes(const std::vector<std::byte>& bytes)
    {
        uint32_t index {0};

        this->_entityId = pfu::ReadUInt32(bytes, index);
        this->_worldName = pfu::ReadString(bytes, index);
        this->_appearanceDetails.Hair = pfu::ReadString(bytes, index);
        this->_appearanceDetails.Body = pfu::ReadString(bytes, index);
        this->_appearanceDetails.ClothesTop = pfu::ReadString(bytes, index);
        this->_appearanceDetails.ClothesBottom = pfu::ReadString(bytes, index);
        this->_appearanceDetails.Feet = pfu::ReadString(bytes, index);
    }

    void ServerClientCharacterSetDetailsPacket::OutputDebugData(std::stringstream& ss) const noexcept
    {
        this->SerializeDebugData(ss, "Entity Id", this->_entityId);
        this->SerializeDebugData(ss, "World Name", this->_worldName);
        this->SerializeDebugData(ss, "Appearance Hair", this->_appearanceDetails.Hair);
        this->SerializeDebugData(ss, "Appearance Hair", this->_appearanceDetails.Body);
        this->SerializeDebugData(ss, "Appearance Hair", this->_appearanceDetails.ClothesTop);
        this->SerializeDebugData(ss, "Appearance Hair", this->_appearanceDetails.ClothesBottom);
        this->SerializeDebugData(ss, "Appearance Hair", this->_appearanceDetails.Feet);
    }
}