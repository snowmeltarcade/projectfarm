#include <SDL_net.h>

#include "networking.h"
#include "api/logging/logging.h"

namespace projectfarm::shared::networking
{
	bool Networking::Initialize()
	{
		api::logging::Log("Initializing networking...");

		if (SDLNet_Init() == -1)
		{
			api::logging::Log("Failed to initialize networking.");
			api::logging::Log(SDL_GetError());
			return false;
		}

		api::logging::Log("Initialized networking.");

		return true;
	}

	void Networking::Shutdown()
	{
		api::logging::Log("Shutting down networking...");

		SDLNet_Quit();

		api::logging::Log("Shut down networking.");
	}
}