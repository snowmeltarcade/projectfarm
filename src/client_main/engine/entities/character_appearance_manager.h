#ifndef PROJECTFARM_CHARACTER_APPEARANCE_MANAGER_H
#define PROJECTFARM_CHARACTER_APPEARANCE_MANAGER_H

#include <cstdint>
#include <filesystem>
#include <memory>
#include <map>
#include <string>

#include <nlohmann/json.hpp>

#include "character.h"
#include "graphics/tile_map.h"
#include "logging/consume_logger.h"
#include "graphics/consume_tile_set_pool.h"

namespace projectfarm::engine::entities
{
    class CharacterAppearanceManager final : public shared::ConsumeLogger,
                                             public graphics::ConsumeTileSetPool
    {
    public:
        struct ActionAnimation
        {
            std::string TileSet;
            uint16_t TileX {0};
            uint16_t TileY {0};
            uint64_t Milliseconds {0};
        };

        using ActionItemType = std::map<shared::entities::CharacterStateValues, std::vector<ActionAnimation>>;

        explicit CharacterAppearanceManager(Character::StateMachineType::StateItemType initialState)
        : _currentState(initialState)
        {
        }
        ~CharacterAppearanceManager() override = default;

        [[nodiscard]] bool LoadCharacterAppearanceFile(const std::filesystem::path& filePath, uint8_t layerIndex) noexcept;

        void ClearCharacterAppearance(uint8_t layerIndex) noexcept;

        void UpdateTileMap(std::shared_ptr<graphics::TileMap>& tileMap,
                           const std::shared_ptr<Character::StateMachineType>& stateMachine,
                           bool force = false) noexcept;

    private:
        Character::StateMachineType::StateItemType _currentState;

        std::map<shared::entities::CharacterStates, std::map<uint8_t, ActionItemType>> _actions;

        static void AddActionItem(const nlohmann::json& json, ActionItemType& actionItem, const std::string& name);
    };
}

#endif
