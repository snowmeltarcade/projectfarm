#ifndef PROJECTFARM_TILE_MAP_TILE_ANIMATION_DATA_H
#define PROJECTFARM_TILE_MAP_TILE_ANIMATION_DATA_H

#include <cstdint>

namespace projectfarm::graphics
{
    class TileMapTileAnimationData final
    {
    public:
        static const uint32_t IndexAbsoluteEmpty;

        TileMapTileAnimationData() = default;
        ~TileMapTileAnimationData() = default;

        void Set(uint32_t tileSetId, uint32_t indexAbsolute, uint64_t milliseconds)
        {
            this->_tileSetId = tileSetId;
            this->_indexAbsolute = indexAbsolute;
            this->_milliseconds = milliseconds;
        }

        [[nodiscard]] uint32_t GetTileSetId() const
        {
            return this->_tileSetId;
        }

        [[nodiscard]] uint32_t GetIndexAbsolute() const
        {
            return this->_indexAbsolute;
        }

        [[nodiscard]] uint64_t GetMilliseconds() const
        {
            return this->_milliseconds;
        }

    private:
        uint32_t _tileSetId = 0;
        uint32_t _indexAbsolute = TileMapTileAnimationData::IndexAbsoluteEmpty;
        uint64_t _milliseconds = 0;
    };
}

#endif
