#ifndef PROJECTFARM_RENDER_H
#define PROJECTFARM_RENDER_H

#include "isystem.h"

namespace projectfarm::shared::game::world::ecs::systems
{
    class Render : public ISystem
    {
    public:
        Render() = default;
        ~Render() override = default;
        Render(const Render&) = delete;
        Render(Render&&) = default;

        void Run() override;
    };
}

#endif
