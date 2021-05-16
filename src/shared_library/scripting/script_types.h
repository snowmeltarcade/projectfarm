#ifndef PROJECTFARM_SCRIPT_TYPES_H
#define PROJECTFARM_SCRIPT_TYPES_H

#include <cstdint>

namespace projectfarm::shared::scripting
{
    enum class ScriptTypes : uint8_t
    {
        Character,
        World,
        UIControl,

        // for use with scripts that are included from other scripts
        Include,
    };
}

#endif
