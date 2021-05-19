#ifndef PROJECTFARM_ACTIONABLE_ENTITY_H
#define PROJECTFARM_ACTIONABLE_ENTITY_H

#include <cstdlib>

#include "engine/action_types.h"

namespace projectfarm::engine
{
    class ActionableEntity
    {
    public:
        ActionableEntity() = default;
        virtual ~ActionableEntity() = default;

        [[nodiscard]] uint32_t GetActionableEntityId() const
        {
            return this->_actionableEntityId;
        }

        virtual void ProcessAction(engine::ActionTypes action, bool actionAdded) noexcept = 0;

        void SetActionableEntityId(uint32_t actionableEntityId)
        {
            this->_actionableEntityId = actionableEntityId;
        }

    private:
        uint32_t _actionableEntityId = 0;
    };
}

#endif
