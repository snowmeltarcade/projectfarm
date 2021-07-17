#ifndef PROJECTFARM_IWORLDINPUTSOURCE_H
#define PROJECTFARM_IWORLDINPUTSOURCE_H

namespace projectfarm::shared::game::world::controllers
{
    class IController
    {
    public:
        virtual void OnMessageReceive() = 0;
    };
}

#endif
