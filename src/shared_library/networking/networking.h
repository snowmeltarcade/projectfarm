#ifndef NETWORKING_H
#define NETWORKING_H

#include "logging/consume_logger.h"

namespace projectfarm::shared::networking
{
	class Networking final : public ConsumeLogger
	{
	public:
		Networking() = default;
		~Networking() override = default;

		Networking(Networking&) = delete;
		Networking(Networking&&) = delete;

		bool Initialize();

		void Shutdown();
	};
}

#endif