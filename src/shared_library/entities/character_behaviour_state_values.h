#ifndef PROJECTFARM_CHARACTER_BEHAVIOUR_STATE_VALUES_H
#define PROJECTFARM_CHARACTER_BEHAVIOUR_STATE_VALUES_H

#include <cstdint>

namespace projectfarm::shared::entities
{
    enum class CharacterBehaviourStateValues : uint8_t
    {
        None,
        Walk,
        Run,
    };
}

#endif
