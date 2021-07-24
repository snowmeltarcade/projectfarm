#include "engine.h"
#include "api/windowing/system.h"
#include "api/logging/logging.h"
#include "concurrency_keys.h"
#include "concurrency/state.h"

using namespace projectfarm::shared::api;

namespace projectfarm::shared::game
{
    bool Engine::Init() noexcept
    {
        logging::Log("Initializing engine...");

        if (!windowing::system::Init())
        {
            logging::Log("Failed to init windowing system.");
            return 1;
        }

        for (auto& game : this->_games)
        {
            if (!game.Initialize())
            {
                logging::Log("Failed to init game: " + game.GetName());
                return false;
            }
        }

        logging::Log("Initialized engine");

        concurrency::state::SetBool(ConcurrencyKeyRunning, true);

        return true;
    }

    bool Engine::Run() noexcept
    {
        logging::Log("Running engine...");

        while (concurrency::state::GetBool(ConcurrencyKeyRunning))
        {
            for (auto &game : this->_games)
            {
                if (!game.RunFrame())
                {
                    logging::Log("Failed to run game: " + game.GetName());
                    return false;
                }
            }

            // render cameras, render targets
            // compose render targets...
        }

        logging::Log("Finished running engine.");

        return true;
    }

    void Engine::Shutdown() noexcept
    {
        logging::Log("Shutting down engine...");

        windowing::system::Shutdown();

        logging::Log("Shut down engine...");
    }
}
