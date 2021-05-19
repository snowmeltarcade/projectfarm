#ifndef SYSTEM_H
#define SYSTEM_H

#include "logging/consume_logger.h"
#include "data/consume_data_provider.h"

#include <thread>

namespace projectfarm::engine
{
	class System final : public shared::ConsumeLogger,
	                     public shared::ConsumeDataProvider
	{
	public:
		System() : _serverStarted(false)
		{}
		~System() override = default;

		void StartServer();
		void StopServer();

		void Shutdown();

	private:
		bool _serverStarted;
	};
}

#endif