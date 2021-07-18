#ifndef PROJECTFARM_ISYSTEM_H
#define PROJECTFARM_ISYSTEM_H

namespace projectfarm::shared::game::world::ecs::systems
{
    class ISystem
    {
    public:
        ISystem() = default;
        virtual ~ISystem() = default;
        ISystem(const ISystem&) = delete;
        ISystem(ISystem&&) = default;

        virtual void Run() = 0;
    };
}

#endif
