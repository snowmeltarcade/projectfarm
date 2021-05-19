#include "render_origin_points.h"
#include "utils/util.h"

namespace projectfarm::graphics
{
    std::optional<RenderOriginPoints> StringToRenderOriginPoints(const std::string& s) noexcept
    {
        auto name = pfu::tolower(s);

        if (s == "center")
        {
            return RenderOriginPoints::Center;
        }
        else if (s == "topleft")
        {
            return RenderOriginPoints::TopLeft;
        }
        else
        {
            return {};
        }
    }
}