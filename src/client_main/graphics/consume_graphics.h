#ifndef CONSUME_GRAPHICS_H
#define CONSUME_GRAPHICS_H

#include <memory>

namespace projectfarm::graphics
{
	class Graphics;

	class ConsumeGraphics
	{
	public:
		ConsumeGraphics() = default;
		virtual ~ConsumeGraphics() = default;

		[[nodiscard]] const std::shared_ptr<Graphics>& GetGraphics() const
		{
			return _graphics;
		}

		void SetGraphics(const std::shared_ptr<Graphics>& graphics)
		{
			this->_graphics = graphics;
		}

	private:
		std::shared_ptr<Graphics> _graphics;
	};
}

#endif