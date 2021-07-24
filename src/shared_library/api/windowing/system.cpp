#include <SDL.h>

#include "system.h"
#include "api/logging/logging.h"

namespace projectfarm::shared::api::windowing::system
{
    bool Init() noexcept
    {
        logging::Log("Setting up SDL...");

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            logging::Log("Failed to initialize SDL.");
            logging::Log(SDL_GetError());
            return false;
        }

        logging::Log("Set up SDL.");

        return true;
    }

    void Shutdown() noexcept
    {
        logging::Log("Shutting down SDL...");

        SDL_Quit();

        logging::Log("Shut down SDL.");
    }
}
