#ifndef PROJECTFARM_MATERIAL_RENDER_DETAILS_H
#define PROJECTFARM_MATERIAL_RENDER_DETAILS_H

#include <string>
#include <memory>
#include <vector>

#include "texture.h"
#include "graphics/colors/color.h"

namespace projectfarm::graphics
{
    struct MaterialRenderDetails
    {
        std::string _name;
        shared::graphics::colors::Color _color {shared::graphics::colors::White};
        std::vector<std::shared_ptr<Texture>> _textures;
    };
}

#endif
