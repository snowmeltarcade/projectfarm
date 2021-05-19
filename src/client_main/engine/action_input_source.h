#ifndef PROJECTFARM_ACTION_INPUT_SOURCE_H
#define PROJECTFARM_ACTION_INPUT_SOURCE_H

#include <vector>
#include <memory>

#include "logging/consume_logger.h"
#include "consume_game.h"
#include "action.h"
#include "engine/action_types.h"

namespace projectfarm::engine
{
    class ActionInputSource : public shared::ConsumeLogger,
                              public engine::ConsumeGame
    {
    public:
        ActionInputSource() = default;
        ~ActionInputSource() override = default;

        virtual void Tick() = 0;

        [[nodiscard]] const std::vector<std::shared_ptr<Action>>& GetActions() const noexcept
        {
            return this->_actions;
        }

        [[nodiscard]] const std::vector<std::shared_ptr<Action>>& GetRemovedActions() const noexcept
        {
            return this->_removedActions;
        }

        void ClearActions() noexcept
        {
            this->_actions.clear();
        }

        void ClearRemovedActions() noexcept
        {
            this->_removedActions.clear();
        }

    protected:
        void AddAction(uint32_t actionableEntityId, engine::ActionTypes actionType) noexcept;
        void RemoveAction(uint32_t actionableEntityId, engine::ActionTypes actionType) noexcept;

        std::vector<std::shared_ptr<Action>> _actions;
        std::vector<std::shared_ptr<Action>> _removedActions;
    };
}

#endif
