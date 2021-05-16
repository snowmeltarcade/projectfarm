#ifndef PROJECTFARM_ENTITY_TYPES_H
#define PROJECTFARM_ENTITY_TYPES_H

#include <cstdint>

namespace projectfarm::shared::entities
{
    enum class EntityTypes : uint8_t
    {
        Unknown = 0,
        World = 1,
        Character = 2,
    };
}

#endif
