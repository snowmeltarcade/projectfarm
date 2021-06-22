#ifndef PROJECTFARM_CSS_SELECTOR_TYPES_H
#define PROJECTFARM_CSS_SELECTOR_TYPES_H

#include <cstdint>

namespace projectfarm::shared::css
{
    enum class CSSSelectorTypes : uint8_t
    {
        // a type of control
        Type,

        // a class to assign to a control
        Class,

        // a control id
        Id,
    };
}

#endif
