#ifndef CONSUME_ACTION_ANIMATIONS_MANAGER_H
#define CONSUME_ACTION_ANIMATIONS_MANAGER_H

#include <memory>

#include "action_animations_manager.h"

namespace projectfarm::engine::entities
{
	class ConsumeActionAnimationsManager
	{
	public:
        ConsumeActionAnimationsManager() = default;
		virtual ~ConsumeActionAnimationsManager() = default;

		void SetActionAnimationsManager(const std::shared_ptr<ActionAnimationsManager>& actionAnimationsManager)
		{
			this->_actionAnimationsManager = actionAnimationsManager;
		}

	protected:
		std::shared_ptr<ActionAnimationsManager> _actionAnimationsManager;
	};
}

#endif
