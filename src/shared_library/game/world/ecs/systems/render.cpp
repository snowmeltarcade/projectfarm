#include "render.h"
#include "api/logging/logging.h"

namespace projectfarm::shared::game::world::ecs::systems
{
    void Render::Run()
    {
        api::logging::Log("Running render system...");

        api::logging::Log("Finished running render system...");
    }
}
