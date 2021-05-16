#ifndef PROJECTFARM_SERVER_CLIENT_CHARACTER_SET_DETAILS_H
#define PROJECTFARM_SERVER_CLIENT_CHARACTER_SET_DETAILS_H

#include <string>

#include "networking/packet.h"
#include "networking/packet_types.h"
#include "entities/entity_types.h"
#include "entities/character_appearance_details.h"

namespace projectfarm::shared::networking::packets
{
    class ServerClientCharacterSetDetailsPacket final : public Packet
    {
    public:
        ServerClientCharacterSetDetailsPacket() = default;
        ~ServerClientCharacterSetDetailsPacket() override = default;

        [[nodiscard]] PacketTypes GetPacketType() const override
        {
            return PacketTypes::ServerClientCharacterSetDetails;
        }

        [[nodiscard]] uint32_t SizeInBytes() const override
        {
            return this->GetSize(this->_entityId) +
                   this->GetSize(this->_worldName) +
                   this->GetSize(this->_appearanceDetails.Hair) +
                   this->GetSize(this->_appearanceDetails.Body) +
                   this->GetSize(this->_appearanceDetails.ClothesTop) +
                   this->GetSize(this->_appearanceDetails.ClothesBottom) +
                   this->GetSize(this->_appearanceDetails.Feet);
        }

        void FromBytes(const std::vector<std::byte>& bytes) override;

        void OutputDebugData(std::stringstream& ss) const noexcept override;

        void SetEntityId(uint32_t entityId) noexcept
        {
            this->_entityId = entityId;
        }

        [[nodiscard]] uint32_t GetEntityId() const noexcept
        {
            return this->_entityId;
        }

        void SetWorldName(const std::string& worldName) noexcept
        {
            this->_worldName = worldName;
        }

        [[nodiscard]] const std::string& GetWorldName() const noexcept
        {
            return this->_worldName;
        }

        void SetAppearanceDetails(const entities::CharacterAppearanceDetails& appearanceDetails) noexcept
        {
            this->_appearanceDetails = appearanceDetails;
        }

        [[nodiscard]] const entities::CharacterAppearanceDetails& GetAppearanceDetails() const noexcept
        {
            return this->_appearanceDetails;
        }

        [[nodiscard]] bool IsVital() const override
        {
            return true;
        }

    protected:
        void SerializeBytes(std::vector<std::byte>& bytes) const noexcept override;

    private:
        uint32_t _entityId {0};
        std::string _worldName;

        entities::CharacterAppearanceDetails _appearanceDetails;
    };
}

#endif
