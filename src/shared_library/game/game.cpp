#include "game.h"
#include "api/logging/logging.h"
#include "world/external_input_source.h"
#include "world/master_logic_input_source.h"

using namespace std::literals;

namespace projectfarm::shared::game
{
    bool Game::Initialize() noexcept
    {
        this->Log("Initializing game...");

        this->Log("Game being created as "s + (this->_isClient ? "client" : "server"));

        if (!this->AddWorlds())
        {
            this->Log("Failed to add worlds.");
            return false;
        }

        this->Log("Initialized game.");

        return true;
    }

    void Game::Run() noexcept
    {
        this->Log("Running game...");

        // this must be run on the same thread as run, so do this here
        if (!this->Initialize())
        {
            this->Log("Failed to initialize.");
            return;
        }

        this->Log("Finished running game.");
    }

    void Game::Shutdown() noexcept
    {
        this->Log("Shutting down game...");

        this->Log("Shut down game.");
    }

    bool Game::AddWorlds() noexcept
    {
        this->_worlds.clear();

        if (this->_isClient)
        {
            // the client only has the one world, which is setup by whatever it receives from the server
            world::World world(std::make_unique<world::ExternalInputSource>());

            this->_worlds.emplace_back(std::move(world));
        }
        else
        {
            // the server has all worlds
            world::World world(std::make_unique<world::MasterLogicInputSource>());

            this->_worlds.emplace_back(std::move(world));
        }

        return true;
    }

    void Game::Log(const std::string& message) const noexcept
    {
        api::logging::Log("Game (" + this->_name + "): " + message);
    }
}
