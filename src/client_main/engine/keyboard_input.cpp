#include <cassert>

#include "keyboard_input.h"

namespace projectfarm::engine
{
	bool KeyboardInput::Initialize() noexcept
	{
		std::fill(std::begin(this->_keyboardKeyStates),
                  std::end(this->_keyboardKeyStates),
                  false);

        this->_keyboardKeyStatesOther.clear();

        std::fill(std::begin(this->_keyboardKeyPressedStates),
                  std::end(this->_keyboardKeyPressedStates),
                  false);

        this->_keyboardKeyPressedStatesOther.clear();

		return true;
	}

    void KeyboardInput::Tick() noexcept
    {
        std::fill(std::begin(this->_keyboardKeyPressedStates),
                  std::end(this->_keyboardKeyPressedStates),
                  false);

        this->_keyboardKeyPressedStatesOther.clear();
    }

	void KeyboardInput::UpdateKeyboardKey(SDL_KeyboardEvent e) noexcept
	{
		auto code = static_cast<int>(e.keysym.sym);
		auto isKeyDown = e.type == SDL_KEYDOWN;

        if (code < 0 || code >= KeyboardInput::NumberOfKeys)
        {
            auto codeString = std::to_string(code);

            this->_keyboardKeyPressedStatesOther[codeString] = isKeyDown &&
                                                               !this->_keyboardKeyStatesOther[codeString];
            this->_keyboardKeyStatesOther[codeString] = isKeyDown;
        }
        else
        {
            this->_keyboardKeyPressedStates[code] = isKeyDown &&
                                                    !this->_keyboardKeyStates[code];
            this->_keyboardKeyStates[code] = isKeyDown;
        }
	}

    bool KeyboardInput::IsKeyDown_Keycode(SDL_Keycode code) noexcept
    {
        int c = static_cast<int>(code);
        return this->IsKeyDown(c);
    }

	bool KeyboardInput::IsKeyDown_Scancode(SDL_Scancode code) noexcept
	{
        return this->IsKeyDown_Keycode(SDL_GetKeyFromScancode(code));
	}

	bool KeyboardInput::IsKeyDown(int code) noexcept
	{
        if (code < 0 || code >= KeyboardInput::NumberOfKeys)
        {
            return this->_keyboardKeyStatesOther[std::to_string(code)];
        }
        else
        {
            return this->_keyboardKeyStates[code];
        }
	}

    bool KeyboardInput::IsKeyPressed_Keycode(SDL_Keycode code) noexcept
    {
        int c = static_cast<int>(code);
        return this->IsKeyPressed(c);
    }

    bool KeyboardInput::IsKeyPressed_Scancode(SDL_Scancode code) noexcept
    {
        return this->IsKeyPressed_Keycode(SDL_GetKeyFromScancode(code));
    }

    bool KeyboardInput::IsKeyPressed(int code) noexcept
    {
        if (code < 0 || code >= KeyboardInput::NumberOfKeys)
        {
            return this->_keyboardKeyPressedStatesOther[std::to_string(code)];
        }
        else
        {
            return this->_keyboardKeyPressedStates[code];
        }
    }

    bool KeyboardInput::IsKeyboardStateEnabled(SDL_Keycode code) const noexcept
    {
        auto state = SDL_GetModState();

        auto desiredState { 0 };

        if (code == SDLK_CAPSLOCK)
        {
            desiredState = KMOD_CAPS;
        }
        else if (code == SDLK_LSHIFT)
        {
            desiredState = KMOD_LSHIFT;
        }
        else if (code == SDLK_RSHIFT)
        {
            desiredState = KMOD_RSHIFT;
        }
        // TODO: Implement other states

        auto result = (state & desiredState) == desiredState;
        return result;
    }

    std::vector<uint32_t> KeyboardInput::GetAllKeysDown() noexcept
    {
	    std::vector<uint32_t> keysDown;

	    for (int i = 0; i < KeyboardInput::NumberOfKeys; ++i)
        {
	        if (this->IsKeyDown_Keycode(static_cast<SDL_Keycode>(i)))
            {
	            keysDown.push_back(static_cast<uint32_t>(i));
            }
        }

	    for (const auto& [code, isDown] : _keyboardKeyStatesOther)
        {
	        if (!isDown)
            {
	            continue;
            }

            keysDown.push_back(static_cast<uint32_t>(std::stoi(code)));
        }

	    return keysDown;
    }
}