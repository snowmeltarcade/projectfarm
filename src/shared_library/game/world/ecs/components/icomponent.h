#ifndef PROJECTFARM_ICOMPONENT_H
#define PROJECTFARM_ICOMPONENT_H

namespace projectfarm::shared::game::world::ecs::components
{
    class IComponent
    {
    public:
        IComponent() = default;
        virtual ~IComponent() = default;
        IComponent(const IComponent&) = delete;
        IComponent(IComponent&&) = default;
    };
}

#endif
