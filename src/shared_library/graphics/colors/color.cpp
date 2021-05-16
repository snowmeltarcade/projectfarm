#include "color.h"

namespace projectfarm::shared::graphics::colors
{
    std::optional<Color> FromString(std::string_view s)
    {
        if (s.empty())
        {
            return {};
        }

        if (s[0] == '#')
        {
            return FromHexString(s);
        }

        auto color = ColorNames.find(std::string(s));
        if (color == ColorNames.end())
        {
            return {};
        }

        auto [_, c] = *color;
        return c;
    }
}