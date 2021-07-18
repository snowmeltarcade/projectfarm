#include "world.h"
#include "api/logging/logging.h"
#include "concurrency/state.h"
#include "game/concurrency_keys.h"

namespace projectfarm::shared::game::world
{
    void World::Run() noexcept
    {
        api::logging::Log("Running world...");

        this->RunECS();

        while (concurrency::state::GetBool(ConcurrencyKeyRunning))
        {
            std::this_thread::yield();
        }

        api::logging::Log("Finished running world.");
    }

    void World::Shutdown()
    {
    }
}