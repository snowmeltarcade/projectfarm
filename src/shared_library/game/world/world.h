#ifndef PROJECTFARM_WORLD_H
#define PROJECTFARM_WORLD_H

#include <memory>

#include "iworld_input_source.h"

namespace projectfarm::shared::game::world
{
    class World final
    {
    public:
        World(std::unique_ptr<IWorldInputSource> inputSource)
            : _inputSource(std::move(inputSource))
        {}
        ~World()
        {
            this->Shutdown();
        }
        World(const World&) = delete;
        World(World&& world) = default;

    private:
        std::unique_ptr<IWorldInputSource> _inputSource;

        void Shutdown();
    };
}

#endif
