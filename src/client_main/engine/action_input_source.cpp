#include "action_input_source.h"

namespace projectfarm::engine
{
    void ActionInputSource::AddAction(uint32_t actionableEntityId, engine::ActionTypes actionType) noexcept
    {
        std::shared_ptr<Action> action = std::make_shared<Action>(actionableEntityId, actionType);
        this->_actions.emplace_back(std::move(action));
    }

    void ActionInputSource::RemoveAction(uint32_t actionableEntityId, engine::ActionTypes actionType) noexcept
    {
        std::shared_ptr<Action> action = std::make_shared<Action>(actionableEntityId, actionType);
        this->_removedActions.emplace_back(std::move(action));
    }
}