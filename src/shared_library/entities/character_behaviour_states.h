#ifndef PROJECTFARM_CHARACTER_BEHAVIOUR_STATES_H
#define PROJECTFARM_CHARACTER_BEHAVIOUR_STATES_H

#include <cstdint>

namespace projectfarm::shared::entities
{
    enum class CharacterBehaviourStates : uint8_t
    {
        Idle,
        MoveTo,
    };
}

#endif
