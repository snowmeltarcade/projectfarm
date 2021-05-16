#ifndef PROJECTFARM_STATE_MACHINE_H
#define PROJECTFARM_STATE_MACHINE_H

#include <list>
#include <string>

#include "state_item.h"

namespace projectfarm::shared::state
{
    template <typename T, typename V = std::string>
    class StateMachine final
    {
    public:
        using StateItemType = StateItem<T, V>;

        explicit StateMachine(StateItemType baseState)
        : _baseState {baseState}
        {}
        ~StateMachine() = default;

        void PushState(StateItemType state) noexcept
        {
            if (!this->_stateStack.empty())
            {
                auto& top = this->_stateStack.front();

                if (top.GetKey() == state.GetKey())
                {
                    top.SetValue(state.GetValue());
                    return;
                }
            }

            this->_stateStack.emplace_front(std::move(state));
        }

        void CancelState(T key) noexcept
        {
            // we want to cancel the topmost instance of the state to cancel
            // and not just the top state. This is because if we are, say, jumping,
            // and we want to cancel the walking state, we don't want to cancel
            // the jump, but cancel the walk so that when the jump is complete,
            // the walk state will be canceled
            for (auto& state : this->_stateStack)
            {
                if (state.GetKey() == key)
                {
                    state.Cancel();
                }
            }
        }

        [[nodiscard]] const StateItemType& GetCurrentState() noexcept
        {
            if (this->_stateStack.size() == 0)
            {
                return this->_baseState;
            }

            do
            {
                auto& top = this->_stateStack.front();

                if (top.GetShouldCancel())
                {
                    this->_stateStack.erase(this->_stateStack.begin());
                    continue;
                }
                else
                {
                    return top;
                }
            } while (this->_stateStack.size() > 0);

            return this->_baseState;
        }

        void ClearStates() noexcept
        {
            this->_stateStack.clear();
        }

    private:
        std::list<StateItemType> _stateStack;

        StateItemType _baseState;
    };
}

#endif