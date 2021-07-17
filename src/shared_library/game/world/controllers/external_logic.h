#ifndef PROJECTFARM_EXTERNAL_LOGIC_H
#define PROJECTFARM_EXTERNAL_LOGIC_H

#include "iworld_controller.h"

namespace projectfarm::shared::game::world::controllers
{
    class ExternalLogic final : public IController
    {
    public:
        ExternalLogic() = default;
        ~ExternalLogic() override = default;
        ExternalLogic(const ExternalLogic&) = delete;
        ExternalLogic(ExternalLogic&&) = default;

        void OnMessageReceive() override;
    };
}

#endif
