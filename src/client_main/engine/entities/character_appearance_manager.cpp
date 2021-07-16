#include <set>

#include "character_appearance_manager.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::entities
{
    bool CharacterAppearanceManager::LoadCharacterAppearanceFile(const std::filesystem::path& filePath, uint8_t layerIndex) noexcept
    {
        // nlohmann::json can throw exceptions
        try
        {
            std::ifstream file(filePath);

            if (!file.is_open())
            {
                shared::api::logging::Log("Failed to open character file: " + filePath.u8string());
                return false;
            }

            nlohmann::json jsonFile;
            file >> jsonFile;

            auto actionsJson = jsonFile["actions"];

            for (const auto& actionJson : actionsJson)
            {
                auto action = actionJson["action"].get<std::string>();

                auto state = projectfarm::shared::entities::StringToCharacterStates(action);
                auto& actionItem = this->_actions[state][layerIndex];

                this->AddActionItem(actionJson, actionItem, "none");
                this->AddActionItem(actionJson, actionItem, "up");
                this->AddActionItem(actionJson, actionItem, "up_left");
                this->AddActionItem(actionJson, actionItem, "up_right");
                this->AddActionItem(actionJson, actionItem, "left");
                this->AddActionItem(actionJson, actionItem, "right");
                this->AddActionItem(actionJson, actionItem, "down");
                this->AddActionItem(actionJson, actionItem, "down_left");
                this->AddActionItem(actionJson, actionItem, "down_right");
            }
        }
        catch (const std::exception& ex)
        {
            shared::api::logging::Log("Failed to read character file: " + filePath.u8string() +
                             "with exception: " + ex.what());

            return false;
        }

        return true;
    }

    void CharacterAppearanceManager::ClearCharacterAppearance(uint8_t layerIndex) noexcept
    {
        for (auto& [_, layers] : this->_actions)
        {
            if (layers.find(layerIndex) != layers.end())
            {
                layers.erase(layerIndex);
            }
        }
    }

    void CharacterAppearanceManager::UpdateTileMap(std::shared_ptr<graphics::TileMap>& tileMap,
                                                   const std::shared_ptr<Character::StateMachineType>& stateMachine,
                                                   bool force) noexcept
    {
        if (!force && this->_currentState == stateMachine->GetCurrentState())
        {
            return;
        }

        tileMap->ClearTileSets();

        this->_currentState = stateMachine->GetCurrentState();

        auto key = this->_currentState.GetKey();
        auto value = this->_currentState.GetValue();

        if (this->_actions.find(key) == this->_actions.end())
        {
            key = shared::entities::CharacterStates::Idle;
        }

        const auto layers = this->_actions[key];

        std::set<std::string> tileSetIndexes;

        for (auto& [_, action] : layers)
        {
            if (action.find(value) == action.end())
            {
                value = shared::entities::CharacterStateValues::None;
            }

            auto actionItems = action.at(value);

            for (const auto& animation : actionItems)
            {
                tileSetIndexes.insert(animation.TileSet);
            }
        }

        uint8_t tileSetId {0u};
        for (const auto& tileSetName : tileSetIndexes)
        {
            auto tileSet = this->_tileSetPool->Get(tileSetName);
            if (tileSet == nullptr)
            {
                shared::api::logging::Log("Failed to get tileset with name: " + tileSetName);
                continue;
            }

            if (!tileMap->SetTileSet(tileSetId++, tileSet))
            {
                shared::api::logging::Log("Failed to set tileset when computing indexes in character appearance with name: " + tileSetName);
                return;
            }
        }

        for (const auto& [layerIndex, action] : layers)
        {
            auto actionItems = action.at(value);

            std::vector<graphics::TileMapTileAnimationData> animationData;

            for (const auto& animation : actionItems)
            {
                graphics::TileMapTileAnimationData data;

                auto tileSetNameIter = tileSetIndexes.find(animation.TileSet);
                if (tileSetNameIter == tileSetIndexes.end())
                {
                    shared::api::logging::Log("Failed to find tileset with name: " + animation.TileSet);
                    continue;
                }

                auto tileSetIndex = std::distance(tileSetIndexes.begin(), tileSetNameIter);

                auto tileSet = this->_tileSetPool->Get(animation.TileSet);
                if (tileSet == nullptr)
                {
                    shared::api::logging::Log("Failed to get tileset with name: " + animation.TileSet);
                    continue;
                }

                data.Set((uint32_t)tileSetIndex,
                         tileSet->GetAbsoluteIndex(animation.TileX, animation.TileY),
                         animation.Milliseconds);

                animationData.emplace_back(data);
            }

            // TODO: Layer with the different layers - hair, feet etc...
            if (!tileMap->SetTileIndex(layerIndex, 0, 0, animationData))
            {
                shared::api::logging::Log("Failed to set tile animation.");
                return;
            }
        }
    }

    void CharacterAppearanceManager::AddActionItem(const nlohmann::json& json, ActionItemType& actionItem, const std::string& name)
    {
        auto item = json.at(name);

        auto animation = item["animation"];

        auto stateValue = projectfarm::shared::entities::CharacterStateValuesToString(name);
        actionItem[stateValue].clear();

        for (const auto& animationItem : animation)
        {
            actionItem[stateValue].push_back(
            {
                animationItem["tileSet"].get<std::string>(),
                animationItem["tileX"].get<uint16_t>(),
                animationItem["tileY"].get<uint16_t>(),
                animationItem["milliseconds"].get<uint64_t>()
            });
        }
    }
}