#ifndef PROJECTFARM_IECS_COMPONENT_H
#define PROJECTFARM_IECS_COMPONENT_H

namespace projectfarm::shared::game::world::ecs
{
    class IECSComponent
    {
    public:
        IECSComponent() = default;
        virtual ~IECSComponent() = default;
        IECSComponent(const IECSComponent&) = delete;
        IECSComponent(IECSComponent&&) = default;
    };
}

#endif
