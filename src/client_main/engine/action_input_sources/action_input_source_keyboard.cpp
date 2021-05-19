#include <SDL.h>

#include "action_input_source_keyboard.h"

#include "engine/game.h"

namespace projectfarm::engine::action_input_sources
{
    void ActionInputSourceKeyboard::Tick()
    {
        if (this->_actionableEntities.empty() ||
            this->_keyMappings.empty())
        {
            return;
        }

        const auto& input = this->GetGame()->GetKeyboardInput();

        auto actionableEntityId = this->_actionableEntities.top()->GetActionableEntityId();

        const auto& keysDown = input->GetAllKeysDown();
        if (keysDown.empty())
        {
            if (!this->_currentBinding._keys.empty())
            {
                this->RemoveAction(actionableEntityId, this->_currentBinding._actionType);
                this->_currentBinding = {};
            }

            return;
        }

        const auto& keyMapping = this->_keyMappings.top();

        const auto& bindings = keyMapping->GetBindings();

        std::vector<KeyMapping::Binding> bindingsToCheck;
        for (const auto& b : bindings)
        {
            if (b._keys.size() == keysDown.size())
            {
                bindingsToCheck.push_back(b);
            }
        }

        if (bindingsToCheck.empty())
        {
            if (!this->_currentBinding._keys.empty())
            {
                this->RemoveAction(actionableEntityId, this->_currentBinding._actionType);
                this->_currentBinding = {};
            }

            return;
        }

        bool bindingHandled {false};

        for (const auto& binding : bindingsToCheck)
        {
            auto allKeysDown = std::all_of(binding._keys.begin(), binding._keys.end(),
                    [&input](const auto& k) { return input->IsKeyDown_Keycode(k); });

            if (allKeysDown)
            {
                if (!this->_currentBinding._keys.empty() && this->_currentBinding != binding)
                {
                    this->RemoveAction(actionableEntityId, this->_currentBinding._actionType);
                }

                this->_currentBinding = binding;

                this->AddAction(actionableEntityId, binding._actionType);

                bindingHandled = true;
            }
        }

        // there must be some keys down that there are no bindings for
        if (!bindingHandled && !this->_currentBinding._keys.empty())
        {
            this->RemoveAction(actionableEntityId, this->_currentBinding._actionType);
            this->_currentBinding = {};
        }
    }
}