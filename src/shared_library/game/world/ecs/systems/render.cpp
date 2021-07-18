#include <thread>

#include "render.h"
#include "api/logging/logging.h"
#include "concurrency/state.h"
#include "game/concurrency_keys.h"

namespace projectfarm::shared::game::world::ecs::systems
{
    void Render::Run()
    {
        api::logging::Log("Running render system...");

        while (concurrency::state::GetBool(ConcurrencyKeyRunning))
        {
            std::this_thread::yield();
        }

        api::logging::Log("Finished running render system...");
    }
}
