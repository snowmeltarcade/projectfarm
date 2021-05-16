#ifndef CONSUME_SCRIPT_SYSTEM_H
#define CONSUME_SCRIPT_SYSTEM_H

#include <string>
#include <memory>
#include <mutex>

#include "script_system.h"

namespace projectfarm::shared::scripting
{
	class ConsumeScriptSystem
	{
	public:
        ConsumeScriptSystem() = default;
		virtual ~ConsumeScriptSystem() = default;

		void SetScriptSystem(const std::shared_ptr<ScriptSystem>& scriptSystem)
		{
			this->_scriptSystem = scriptSystem;
		}

	protected:
		std::shared_ptr<ScriptSystem> _scriptSystem;
	};
}

#endif
