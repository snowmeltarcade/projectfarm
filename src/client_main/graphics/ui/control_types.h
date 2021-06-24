#ifndef PROJECTFARM_CONTROL_TYPES_H
#define PROJECTFARM_CONTROL_TYPES_H

#include <string>

namespace projectfarm::graphics::ui
{
    enum class ControlTypes : uint8_t
    {
        Custom,
        Canvas,
        Label,
        MouseCursor,
        Texture,
    };

    std::string ControlTypesToString(ControlTypes type) noexcept;
}

#endif
