#ifndef PROJECTFARM_CHARACTER_APPEARANCE_DETAILS_H
#define PROJECTFARM_CHARACTER_APPEARANCE_DETAILS_H

#include <string>

namespace projectfarm::shared::entities
{
    struct CharacterAppearanceDetails
    {
        std::string Hair;
        std::string Body;
        std::string ClothesTop;
        std::string ClothesBottom;
        std::string Feet;
    };
}

#endif
