#include "character_state_values.h"

namespace projectfarm::shared::entities
{
    CharacterStateValues CharacterStateValuesToString(const std::string& s)
    {
        if (s == "left" || s == "Left")
        {
            return CharacterStateValues::Left;
        }
        else if (s == "right" || s == "Right")
        {
            return CharacterStateValues::Right;
        }
        else if (s == "up" || s == "Up")
        {
            return CharacterStateValues::Up;
        }
        else if (s == "down" || s == "Down")
        {
            return CharacterStateValues::Down;
        }
        else if (s == "up_left" || s == "UpLeft")
        {
            return CharacterStateValues::UpLeft;
        }
        else if (s == "up_right" || s == "UpRight")
        {
            return CharacterStateValues::UpRight;
        }
        else if (s == "down_left" || s == "DownLeft")
        {
            return CharacterStateValues::DownLeft;
        }
        else if (s == "down_right" || s == "DownRight")
        {
            return CharacterStateValues::DownRight;
        }

        return CharacterStateValues::None;
    }
}