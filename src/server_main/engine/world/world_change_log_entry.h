#ifndef PROJECTFARM_WORLD_CHANGE_LOG_ENTRY_H
#define PROJECTFARM_WORLD_CHANGE_LOG_ENTRY_H

#include <cstdint>

namespace projectfarm::engine::world
{
    struct WorldChangeLogEntry
    {
        uint8_t LayerIndex {0};

        uint32_t TileX {0};
        uint32_t TileY {0};

        uint64_t Time {0};

        uint16_t PlotIndex {0};
    };
}

#endif
