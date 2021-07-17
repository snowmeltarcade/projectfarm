#ifndef PROJECTFARM_MASTER_LOGIC_H
#define PROJECTFARM_MASTER_LOGIC_H

#include "iworld_controller.h"

namespace projectfarm::shared::game::world::controllers
{
    class MasterLogic final : public IController
    {
    public:
        void OnMessageReceive() override;
    };
}

#endif
