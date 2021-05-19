#ifndef PROJECTFARM_RENDER_ORIGIN_POINTS_H
#define PROJECTFARM_RENDER_ORIGIN_POINTS_H

#include <cstdlib>
#include <optional>
#include <string>

namespace projectfarm::graphics
{
    enum class RenderOriginPoints : uint8_t
    {
        Center = 0,
        TopLeft = 1,
    };

    std::optional<RenderOriginPoints> StringToRenderOriginPoints(const std::string& s) noexcept;
}

#endif
