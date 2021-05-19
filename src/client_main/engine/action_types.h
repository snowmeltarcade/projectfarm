#ifndef PROJECTFARM_CHARACTER_ACTIONS_H
#define PROJECTFARM_CHARACTER_ACTIONS_H

#include <cstdint>

namespace projectfarm::engine
{
    enum class ActionTypes : uint8_t
    {
        None,
        Idle,
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,
        MoveUpLeft,
        MoveUpRight,
        MoveDownLeft,
        MoveDownRight,
        MoveLeftFast,
        MoveRightFast,
        MoveUpFast,
        MoveDownFast,
        MoveUpLeftFast,
        MoveUpRightFast,
        MoveDownLeftFast,
        MoveDownRightFast,
    };
}

#endif
