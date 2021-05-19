#ifndef CONSUME_FONT_MANAGER_H
#define CONSUME_FONT_MANAGER_H

#include <memory>

#include "graphics/font_manager.h"

namespace projectfarm::graphics
{
	class ConsumeFontManager
	{
	public:
        ConsumeFontManager() = default;
		virtual ~ConsumeFontManager() = default;

		void SetFontManager(const std::shared_ptr<FontManager>& fontManager)
		{
			this->_fontManager = fontManager;
		}

	protected:
		std::shared_ptr<FontManager> _fontManager;
	};
}

#endif