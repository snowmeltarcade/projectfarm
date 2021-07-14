#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <array>
#include <vector>
#include <unordered_map>
#include <SDL.h>

namespace projectfarm::engine
{
	class KeyboardInput final
	{
	public:
        KeyboardInput() = default;
		~KeyboardInput() = default;

		bool Initialize() noexcept;

        void Tick() noexcept;

		void UpdateKeyboardKey(SDL_KeyboardEvent e) noexcept;

        [[nodiscard]] bool IsKeyDown_Keycode(SDL_Keycode code) noexcept;
		[[nodiscard]] bool IsKeyDown_Scancode(SDL_Scancode code) noexcept;
		[[nodiscard]] bool IsKeyDown(int code) noexcept;

        [[nodiscard]] bool IsKeyPressed_Keycode(SDL_Keycode code) noexcept;
        [[nodiscard]] bool IsKeyPressed_Scancode(SDL_Scancode code) noexcept;
        [[nodiscard]] bool IsKeyPressed(int code) noexcept;

        [[nodiscard]] bool IsKeyboardStateEnabled(int code) const noexcept;

		[[nodiscard]] std::vector<uint32_t> GetAllKeysDown() noexcept;

	private:
		static constexpr int NumberOfKeys = 128; // the number of keycodes we can accept
                                                 // ignore everything after 127 - see https://wiki.libsdl.org/SDLKeycodeLookup

        // true if a key is currently down, false if up
		std::array<bool, KeyboardInput::NumberOfKeys> _keyboardKeyStates {};

        // for keys outside of the NumberOfKeys range
        std::unordered_map<std::string, bool> _keyboardKeyStatesOther;

        // true if a key is pressed, then set to false on the next tick
        std::array<bool, KeyboardInput::NumberOfKeys> _keyboardKeyPressedStates {};

        // for keys outside of the NumberOfKeys range
        std::unordered_map<std::string, bool> _keyboardKeyPressedStatesOther;
	};
}

#endif