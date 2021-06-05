#ifndef SYSTEM_H
#define SYSTEM_H

#include <thread>

#include "logging/consume_logger.h"
#include "data/consume_data_provider.h"
#include "platform.h"

namespace projectfarm::engine
{
	class System final : public shared::ConsumeLogger,
	                     public shared::ConsumeDataProvider
	{
	public:
		System() : _serverStarted(false)
		{}
		~System() override = default;

		void Initialize() noexcept;

		void StartServer();
		void StopServer();

		void Shutdown();

		void LogSystemInfo() noexcept;

	private:
		bool _serverStarted;

		PlatformTypes _platformType {PlatformTypes::Unknown};
	};
}

#endif