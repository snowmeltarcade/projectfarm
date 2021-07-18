#ifndef PROJECTFARM_WORLD_H
#define PROJECTFARM_WORLD_H

#include <memory>

#include "controllers/iworld_controller.h"
#include "ecs/ecs.h"
#include "ecs/systems/isystem.h"

namespace projectfarm::shared::game::world
{
    class World final
    {
    public:
        World(std::unique_ptr<controllers::IController> controller)
            : _controller(std::move(controller))
        {}
        ~World()
        {
            this->Shutdown();
        }
        World(const World&) = delete;
        World(World&& world) = default;

        void AddECSSystem(std::unique_ptr<ecs::systems::ISystem> system) noexcept
        {
            this->_ecs.AddSystem(std::move(system));
        }

        void Run() noexcept;

    private:
        std::unique_ptr<controllers::IController> _controller;

        ecs::ECS _ecs;

        void Shutdown();

        void RunECS() noexcept
        {
            this->_ecs.RunSystems();
        }
    };
}

#endif
