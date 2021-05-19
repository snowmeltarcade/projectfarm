#include "tile_map_tile.h"

namespace projectfarm::graphics
{
    void TileMapTile::Tick(uint64_t frameTime)
    {
        if (this->_numberOfFrames <= 1)
        {
            return;
        }

        this->_currentAnimationTime += frameTime;

        if (this->_currentAnimationTime > this->GetMilliseconds())
        {
            this->_currentFrame++;
            this->_currentAnimationTime = 0;

            if (this->_currentFrame >= this->_numberOfFrames)
            {
                this->_currentFrame = 0;
            }
        }
    }

    bool TileMapTile::operator== (const TileMapTile& other) const noexcept
    {
        if (other.GetNumberOfFrames() != this->GetNumberOfFrames())
        {
            return false;
        }

        for (auto i = 0u; i < this->GetNumberOfFrames(); ++i)
        {
            if (this->_frames[i].GetTileSetId() != other._frames[i].GetTileSetId() ||
                this->_frames[i].GetIndexAbsolute() != other._frames[i].GetIndexAbsolute() ||
                this->_frames[i].GetMilliseconds() != other._frames[i].GetMilliseconds())
            {
                return false;
            }
        }

        return true;
    }

    bool TileMapTile::operator== (const std::vector<TileMapTileAnimationData>& other) const noexcept
    {
        if (other.size() != this->GetNumberOfFrames())
        {
            return false;
        }

        for (auto i = 0u; i < this->GetNumberOfFrames(); ++i)
        {
            if (this->_frames[i].GetTileSetId() != other[i].GetTileSetId() ||
                this->_frames[i].GetIndexAbsolute() != other[i].GetIndexAbsolute() ||
                this->_frames[i].GetMilliseconds() != other[i].GetMilliseconds())
            {
                return false;
            }
        }

        return true;
    }
}