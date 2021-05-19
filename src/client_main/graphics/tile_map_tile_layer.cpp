#include "tile_map_tile_layer.h"
#include "tile_map.h"

namespace projectfarm::graphics
{
    bool TileMapTileLayer::Setup(uint32_t widthInTiles, uint32_t heightInTiles)
    {
        this->_tiles.clear();

        this->_tiles.reserve(widthInTiles);

        for (auto x = 0u; x < widthInTiles; ++x)
        {
            std::vector<uint16_t> layer(heightInTiles);

            std::fill(layer.begin(), layer.end(), TileMap::EmptyTileIndex);

            this->_tiles.emplace_back(std::move(layer));
        }

        return true;
    }
}