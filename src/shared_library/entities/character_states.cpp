#include "character_states.h"

namespace projectfarm::shared::entities
{
    CharacterStates StringToCharacterStates(std::string_view str)
    {
        auto s = projectfarm::shared::utils::tolower(str);

        if (s == "idle")
        {
            return CharacterStates::Idle;
        }
        else if (s == "walk")
        {
            return CharacterStates::Walk;
        }
        else if (s == "run")
        {
            return CharacterStates::Run;
        }

        return CharacterStates::Idle;
    }
}