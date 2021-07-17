#ifndef PROJECTFARM_ECS_H
#define PROJECTFARM_ECS_H

#include <vector>
#include <memory>
#include <future>

#include "systems/isystem.h"

namespace projectfarm::shared::game::world::ecs
{
    class ECS final
    {
    public:
        ECS() = default;
        ~ECS()
        {
            this->Shutdown();
        }
        ECS(const ECS&) = delete;
        ECS(ECS&&) = default;

        void AddSystem(std::unique_ptr<systems::ISystem> system) noexcept
        {
            this->_systems.emplace_back(std::move(system));
        }

        void RunSystems() noexcept;

    private:
        std::vector<std::unique_ptr<systems::ISystem>> _systems;
        std::vector<std::future<void>> _systemPromises;

        void Shutdown() noexcept;
    };
}

#endif
