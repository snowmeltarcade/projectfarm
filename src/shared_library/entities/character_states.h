#ifndef PROJECTFARM_CHARACTER_STATES_H
#define PROJECTFARM_CHARACTER_STATES_H

#include <cstdint>
#include <string>

#include "utils/strings.h"

namespace projectfarm::shared::entities
{
    enum class CharacterStates : uint8_t
    {
        Idle,
        Walk,
        Run,
    };

    CharacterStates StringToCharacterStates(std::string_view str);
}

#endif
