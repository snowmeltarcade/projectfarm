#ifndef PROJECTFARM_MOUSE_INPUT_H
#define PROJECTFARM_MOUSE_INPUT_H

#include <algorithm>
#include <SDL.h>

namespace projectfarm::engine
{
    class MouseInput final
    {
    public:
        MouseInput() = default;
        ~MouseInput() = default;

        void SetPosition(uint32_t x, uint32_t y) noexcept
        {
            this->_x = x;
            this->_y = y;
        }

        [[nodiscard]]
        std::pair<uint32_t, uint32_t> GetPosition() const noexcept
        {
            return {this->_x, this->_y};
        }

        void SetButtonState(bool state) noexcept
        {
            if (!this->_buttonState && state)
            {
                this->_isButtonClicked = true;
            }
            else if (!state)
            {
                this->_isButtonClicked = false;
            }

            this->_buttonState = state;
        }

        [[nodiscard]]
        bool IsButtonClicked() const noexcept
        {
            return this->_isButtonClicked;
        }

        void ResetButtonClicked() noexcept
        {
            this->_isButtonClicked = false;
        }

        [[nodiscard]]
        bool IsButtonDown() const noexcept
        {
            return this->_buttonState;
        }

    private:
        uint32_t _x {0};
        uint32_t _y {0};

        bool _buttonState {false};
        bool _isButtonClicked {false};
    };
}

#endif
