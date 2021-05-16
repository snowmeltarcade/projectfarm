#ifndef PROJECTFARM_CHARACTER_STATE_VALUES_H
#define PROJECTFARM_CHARACTER_STATE_VALUES_H

#include <cstdint>
#include <string>

namespace projectfarm::shared::entities
{
    enum class CharacterStateValues : uint8_t
    {
        None,
        Left,
        Right,
        Up,
        Down,
        UpLeft,
        UpRight,
        DownLeft,
        DownRight,
    };

    CharacterStateValues CharacterStateValuesToString(const std::string& s);
}

#endif
