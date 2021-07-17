#include "game.h"
#include "api/logging/logging.h"

using namespace std::literals;

namespace projectfarm::shared::game
{
    bool Game::Initialize() noexcept
    {
        this->Log("Initializing game...");

        this->Log("Game being created as "s + (this->_isClient ? "client" : "server"));

        this->Log("Initialized game.");

        return true;
    }

    void Game::Run() noexcept
    {
        this->Log("Running game...");

        this->Log("Run game.");
    }

    void Game::Shutdown() noexcept
    {
        this->Log("Shutting down game...");

        this->Log("Shut down game.");
    }

    void Game::Log(const std::string& message) const noexcept
    {
        api::logging::Log("Game (" + this->_name + "): " + message);
    }
}
