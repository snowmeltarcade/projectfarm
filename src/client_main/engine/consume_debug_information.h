#ifndef CONSUME_DEBUG_INFORMATION_H
#define CONSUME_DEBUG_INFORMATION_H

#include <memory>

namespace projectfarm::engine
{
	class DebugInformation;

	class ConsumeDebugInformation
	{
	public:
		ConsumeDebugInformation() = default;
		virtual ~ConsumeDebugInformation() = default;

        [[nodiscard]] const std::shared_ptr<DebugInformation>& GetDebugInformation() const
		{
			return _debugInformation;
		}

		void SetDebugInformation(const std::shared_ptr<DebugInformation>& game)
		{
			this->_debugInformation = game;
		}

	private:
		std::shared_ptr<DebugInformation> _debugInformation;
	};
}

#endif
