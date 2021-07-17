#ifndef PROJECTFARM_IWORLDINPUTSOURCE_H
#define PROJECTFARM_IWORLDINPUTSOURCE_H

namespace projectfarm::shared::game::world::controllers
{
    class IController
    {
    public:
        IController() = default;
        virtual ~IController() = default;
        IController(const IController&) = delete;
        IController(IController&&) = default;

        virtual void OnMessageReceive() = 0;
    };
}

#endif
