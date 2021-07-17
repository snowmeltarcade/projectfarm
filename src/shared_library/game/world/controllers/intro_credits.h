#ifndef PROJECTFARM_EXTERNAL_LOGIC_H
#define PROJECTFARM_EXTERNAL_LOGIC_H

#include "iworld_controller.h"

namespace projectfarm::shared::game::world::controllers
{
    class IntroCredits final : public IController
    {
    public:
        void OnMessageReceive() override;
    };
}

#endif
