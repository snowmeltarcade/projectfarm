#ifndef PROJECTFARM_TILE_MAP_TILE_LAYER_H
#define PROJECTFARM_TILE_MAP_TILE_LAYER_H

#include <vector>
#include <memory>

#include "tile_map_tile.h"

namespace projectfarm::graphics
{
    class TileMapTileLayer final
    {
    public:
        TileMapTileLayer() = default;
        ~TileMapTileLayer() = default;

        [[nodiscard]] bool Setup(uint32_t widthInTiles, uint32_t heightInTiles);

        [[nodiscard]] uint16_t GetTile(uint32_t indexX, uint32_t indexY)
        {
            return this->_tiles[indexX][indexY];
        }

        void SetTile(uint32_t indexX, uint32_t indexY, uint16_t tileIndex)
        {
            this->_tiles[indexX][indexY] = tileIndex;
        }

    private:
        std::vector<std::vector<uint16_t>> _tiles;
    };
}

#endif
