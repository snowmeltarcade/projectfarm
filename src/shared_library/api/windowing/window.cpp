#include <SDL.h>

#include "window.h"
#include "api/logging/logging.h"

namespace projectfarm::shared::api::windowing
{
    bool Window::LoadFromConfig(const std::filesystem::path& configPath) noexcept
    {
        if (!this->SetupSDL())
        {
            logging::Log("Failed to setup SDL.");
            return false;
        }

        return true;
    };

    bool Window::SetupSDL() noexcept
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

    void Window::ShutdownSDL() noexcept
    {
        logging::Log("Shutting down SDL...");

        SDL_Quit();

        logging::Log("Shut down SDL.");
    }
}
