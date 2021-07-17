#include "world.h"
#include "api/logging/logging.h"

namespace projectfarm::shared::game::world
{
    void World::Run() noexcept
    {
        api::logging::Log("Running world...");

        this->RunECS();

        api::logging::Log("Finished running world.");
    }

    void World::Shutdown()
    {
    }
}