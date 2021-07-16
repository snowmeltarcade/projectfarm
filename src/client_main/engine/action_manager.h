#ifndef PROJECTFARM_ACTION_MANAGER_H
#define PROJECTFARM_ACTION_MANAGER_H

#include <vector>
#include <memory>

#include "consume_game.h"

#include "action_input_source.h"
#include "actionable_entity.h"

namespace projectfarm::engine
{
    class ActionManager final : public engine::ConsumeGame
    {
    public:
        ActionManager() = default;
        ~ActionManager() override = default;

        [[nodiscard]]
        bool Initialize();

        void Shutdown();

        void ProcessActions();

        void RegisterActionableEntity(const std::shared_ptr<ActionableEntity>& actionableEntity);
        std::shared_ptr<ActionableEntity> GetActionableEntityById(uint32_t actionableEntityId);

        template<typename T>
        std::shared_ptr<T> GetInputSourceOfType()
        {
            for (const auto& source : this->_inputSources)
            {
                if (dynamic_cast<const T*>(source.get()))
                {
                    return std::dynamic_pointer_cast<T>(source);
                }
            }

            return {};
        }

    private:
        std::vector<std::shared_ptr<ActionInputSource>> _inputSources;
        std::vector<std::shared_ptr<ActionableEntity>> _actionableEntities;

        uint32_t _actionableEntityIdIndex = 0;

        [[nodiscard]] uint32_t GenerateNewActionableEntityId()
        {
            return ++this->_actionableEntityIdIndex;
        }
    };
}

#endif
