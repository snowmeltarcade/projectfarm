#ifndef PROJECTFARM_WORLD_H
#define PROJECTFARM_WORLD_H

#include <memory>

#include "controllers/iworld_controller.h"

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

    private:
        std::unique_ptr<controllers::IController> _controller;

        void Shutdown();
    };
}

#endif
