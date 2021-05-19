#ifndef PROJECTFARM_ACTION_H
#define PROJECTFARM_ACTION_H

#include <cstdlib>

#include "action_types.h"

namespace projectfarm::engine
{
    class Action final
    {
    public:
        Action(uint32_t actionableEntityId, ActionTypes action)
        : _actionableEntityId(actionableEntityId),
          _action(action)
        {}
        ~Action() = default;

        [[nodiscard]] uint32_t GetActionableEntityId() const
        {
            return this->_actionableEntityId;
        }

        [[nodiscard]] ActionTypes GetAction() const
        {
            return this->_action;
        }

    private:
        uint32_t _actionableEntityId {0};
        ActionTypes _action {ActionTypes::Idle};
    };
}

#endif
