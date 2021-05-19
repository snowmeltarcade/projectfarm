#ifndef CONSUME_UI_H
#define CONSUME_UI_H

#include <memory>

namespace projectfarm::graphics::ui
{
    class UI;

	class ConsumeUI
	{
	public:
        ConsumeUI() = default;
		virtual ~ConsumeUI() = default;

		void SetUI(const std::shared_ptr<UI>& ui)
		{
			this->_ui = ui;
		}

	protected:
		std::shared_ptr<UI> _ui;
	};
}

#endif