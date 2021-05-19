#ifndef CONSUME_MOUSE_INPUT_H
#define CONSUME_MOUSE_INPUT_H

#include <memory>

namespace projectfarm::engine
{
	class MouseInput;

	class ConsumeMouseInput
	{
	public:
        explicit ConsumeMouseInput(std::shared_ptr<MouseInput> mouseInput)
        : _mouseInput(std::move(mouseInput))
        {
        }
		virtual ~ConsumeMouseInput() = default;

        [[nodiscard]] const std::shared_ptr<MouseInput>& GetMouseInput() const noexcept
		{
			return _mouseInput;
		}

	private:
		std::shared_ptr<MouseInput> _mouseInput;
	};
}

#endif
