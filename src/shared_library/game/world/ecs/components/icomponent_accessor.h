#ifndef PROJECTFARM_ICOMPONENT_ACCESSOR_H
#define PROJECTFARM_ICOMPONENT_ACCESSOR_H

namespace projectfarm::shared::game::world::ecs::components
{
    // we don't want to expose the whole ECS class with multiple threads,
    // so this will expose only APIs that will handle system <-> component communication
    class IComponentAccessor
    {
    public:
        IComponentAccessor() = default;
        virtual ~IComponentAccessor() = default;
        IComponentAccessor(const IComponentAccessor&) = delete;
        IComponentAccessor(IComponentAccessor&&) = default;

        // this will create a copy of the requested components
        virtual void ReadComponents() noexcept = 0;

        // this will write the passed components to the ecs
        // it will first lock the components
        virtual void WriteComponents() noexcept = 0;
    };
}

#endif
