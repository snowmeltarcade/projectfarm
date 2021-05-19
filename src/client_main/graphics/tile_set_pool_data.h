#ifndef PROJECTFARM_TILE_SET_POOL_DATA_H
#define PROJECTFARM_TILE_SET_POOL_DATA_H

#include "tile_set.h"

#include <memory>
#include <cstdint>

namespace projectfarm::graphics
{
    class TileSetPoolData final
    {
    public:
        std::shared_ptr<TileSet> TileSetData;
        uint32_t ReferenceCount;
    };
}

#endif
