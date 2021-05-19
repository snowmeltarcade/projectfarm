#ifndef PROJECTFARM_ACTION_INPUT_SOURCE_KEYBOARD_H
#define PROJECTFARM_ACTION_INPUT_SOURCE_KEYBOARD_H

#include <stack>
#include <memory>

#include "engine/action_input_source.h"
#include "engine/actionable_entity.h"
#include "key_mapping.h"

namespace projectfarm::engine::action_input_sources
{
    class ActionInputSourceKeyboard final : public ActionInputSource
    {
    public:
        ActionInputSourceKeyboard() = default;
        ~ActionInputSourceKeyboard() override = default;

        void Tick() override;

        void PushActionableEntity(const std::shared_ptr<engine::ActionableEntity>& actionableEntity) noexcept
        {
            this->_actionableEntities.push(actionableEntity);
        }

//        void PopActionableEntity()
//        {
//            this->_actionableEntities.pop();
//        }

        void PushKeyMapping(const std::shared_ptr<KeyMapping>& mapping) noexcept
        {
            this->_keyMappings.push(mapping);
        }

        void PopKeyMapping()
        {
            if (!this->_keyMappings.empty())
            {
                this->_keyMappings.pop();
            }
        }

        void Clear() noexcept
        {
            this->_actionableEntities = {};
            this->_keyMappings = {};
            this->_currentBinding = {};
        }

    private:
        std::stack<std::shared_ptr<engine::ActionableEntity>> _actionableEntities;
        std::stack<std::shared_ptr<KeyMapping>> _keyMappings;

        KeyMapping::Binding _currentBinding;
    };
}

#endif
