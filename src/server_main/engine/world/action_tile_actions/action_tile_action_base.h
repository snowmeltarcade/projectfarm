#ifndef PROJECTFARM_ACTION_TILE_ACTION_BASE_H
#define PROJECTFARM_ACTION_TILE_ACTION_BASE_H

namespace projectfarm::engine::world::action_tile_actions
{
    class ActionTileActionBase
    {
    public:
        ~ActionTileActionBase() = default;

        [[nodiscard]]
        virtual bool Run() noexcept = 0;
    };
}

#endif
