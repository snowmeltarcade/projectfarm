#include "ecs.h"
#include "api/logging/logging.h"

namespace projectfarm::shared::game::world::ecs
{
    void ECS::RunSystems() noexcept
    {
        api::logging::Log("Start ECS systems...");

        for (const auto& system : this->_systems)
        {
            auto promise = std::async(std::launch::async, &systems::ISystem::Run, system.get());
            this->_systemPromises.emplace_back(std::move(promise));
        }

        api::logging::Log("Started ECS systems.");
    }

    void ECS::ReadComponents() noexcept
    {
    }

    void ECS::WriteComponents() noexcept
    {
    }

    void ECS::Shutdown() noexcept
    {
        api::logging::Log("Waiting for ECS systems to stop...");

        for (auto& promise : this->_systemPromises)
        {
            promise.wait();
        }

        api::logging::Log("ECS systems stopped.");
    }
}