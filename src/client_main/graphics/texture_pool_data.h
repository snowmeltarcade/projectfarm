#ifndef PROJECTFARM_TEXTURE_POOL_DATA_H
#define PROJECTFARM_TEXTURE_POOL_DATA_H

#include <cstdint>

#include "graphics_dependencies.h"

namespace projectfarm::graphics
{
    struct TexturePoolData final
    {
        TexturePoolData() = default;
        ~TexturePoolData() = default;

        [[nodiscard]] bool IsEmpty() const noexcept
        {
            return this->TextureId == 0 && this->ReferenceCount == 0;
        }

        GLuint TextureId {0};
        uint32_t Width {0};
        uint32_t Height {0};
        uint32_t ReferenceCount {0};
    };
}

#endif
