#ifndef PROJECTFARM_COLOR_HSV_H
#define PROJECTFARM_COLOR_HSV_H

#include <string>
#include <optional>

namespace projectfarm::shared::graphics::colors
{
    struct ColorHSV
    {
        float h {0.0f};
        float s {0.0f};
        float v {0.0f};
    };

    std::optional<ColorHSV> FromStringHSV(std::string_view s);
}

#endif
