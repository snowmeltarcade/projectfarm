#include "networking.h"

#include <SDL_net.h>

namespace projectfarm::shared::networking
{
	bool Networking::Initialize()
	{
		this->LogMessage("Initializing networking...");

		if (SDLNet_Init() == -1)
		{
			this->LogMessage("Failed to initialize networking.");
			this->LogMessage(SDL_GetError());
			return false;
		}

		this->LogMessage("Initialized networking.");

		return true;
	}

	void Networking::Shutdown()
	{
		this->LogMessage("Shutting down networking...");

		SDLNet_Quit();

		this->LogMessage("Shut down networking.");
	}
}