#include "action_manager.h"
#include "action_input_sources/action_input_source_keyboard.h"
#include "api/logging/logging.h"

namespace projectfarm::engine
{
    bool ActionManager::Initialize()
    {
        shared::api::logging::Log("Initializing action manager...");

        auto keyboardSource = std::make_shared<action_input_sources::ActionInputSourceKeyboard>();
        keyboardSource->SetGame(this->GetGame());

        this->_inputSources.emplace_back(std::move(keyboardSource));

        shared::api::logging::Log("Initialized action manager.");

        return true;
    }

    void ActionManager::Shutdown()
    {
        shared::api::logging::Log("Shutting down action manager...");

        this->_actionableEntities.clear();
        this->_inputSources.clear();

        shared::api::logging::Log("Shut down action manager.");
    }

    void ActionManager::ProcessActions()
    {
        for (const auto& source : this->_inputSources)
        {
            source->Tick();

            const auto& actions = source->GetActions();

            for (const auto& action : actions)
            {
                auto id = action->GetActionableEntityId();
                const auto& entity = this->GetActionableEntityById(id);
                if (entity == nullptr)
                {
                    shared::api::logging::Log("Failed to find actionable entity with id: " + std::to_string(id));
                    continue;
                }

                entity->ProcessAction(action->GetAction(), true);
            }

            source->ClearActions();

            const auto& removedActions = source->GetRemovedActions();

            for (const auto& action : removedActions)
            {
                auto id = action->GetActionableEntityId();
                const auto& entity = this->GetActionableEntityById(id);
                if (entity == nullptr)
                {
                    shared::api::logging::Log("Failed to find actionable entity with id: " + std::to_string(id));
                    continue;
                }

                entity->ProcessAction(action->GetAction(), false);
            }

            source->ClearRemovedActions();
        }
    }

    void ActionManager::RegisterActionableEntity(const std::shared_ptr<ActionableEntity>& actionableEntity)
    {
        actionableEntity->SetActionableEntityId(this->GenerateNewActionableEntityId());

        this->_actionableEntities.push_back(actionableEntity);
    }

    std::shared_ptr<ActionableEntity> ActionManager::GetActionableEntityById(uint32_t actionableEntityId)
    {
        for (const auto& entity : this->_actionableEntities)
        {
            if (entity->GetActionableEntityId() == actionableEntityId)
            {
                return entity;
            }
        }

        return nullptr;
    }
}