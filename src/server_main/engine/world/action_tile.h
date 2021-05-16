#ifndef PROJECTFARM_ACTION_TILE_H
#define PROJECTFARM_ACTION_TILE_H

#include <cstdint>
#include <string>
#include <unordered_map>

namespace projectfarm::engine::world
{
    struct ActionTile final
    {
        uint32_t X {0};
        uint32_t Y {0};

        std::unordered_map<std::string, std::string> Properties;
    };
}

#endif
