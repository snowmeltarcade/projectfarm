#ifndef PROJECTFARM_ACTION_TILE_ACTION_BASE_H
#define PROJECTFARM_ACTION_TILE_ACTION_BASE_H

#include "logging/consume_logger.h"

namespace projectfarm::engine::world::action_tile_actions
{
    class ActionTileActionBase : public shared::ConsumeLogger
    {
    public:
        ~ActionTileActionBase() override = default;

        [[nodiscard]] virtual bool Run() noexcept = 0;
    };
}

#endif
