#ifndef PROJECTFARM_ECS_H
#define PROJECTFARM_ECS_H

#include <vector>
#include <memory>
#include <future>

#include "systems/isystem.h"
#include "components/icomponent_accessor.h"

namespace projectfarm::shared::game::world::ecs
{
    class ECS final : public components::IComponentAccessor
    {
    public:
        ECS() = default;
        ~ECS() override
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

        void ReadComponents() noexcept override;

        void WriteComponents() noexcept override;

    private:
        std::vector<std::unique_ptr<systems::ISystem>> _systems;
        std::vector<std::future<void>> _systemPromises;

        void Shutdown() noexcept;
    };
}

#endif
