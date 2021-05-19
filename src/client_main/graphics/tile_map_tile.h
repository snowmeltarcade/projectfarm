#ifndef PROJECTFARM_TILE_MAP_TILE_H
#define PROJECTFARM_TILE_MAP_TILE_H

#include <cstdint>
#include <vector>

#include "logging/consume_logger.h"
#include "tile_map_tile_animation_data.h"

namespace projectfarm::graphics
{
    class TileMapTile final : public shared::ConsumeLogger
    {
    public:
        explicit TileMapTile(uint32_t numberOfFrames)
        {
            this->_numberOfFrames = numberOfFrames;
            this->_frames.resize(numberOfFrames);
        }
        ~TileMapTile() override = default;

        [[nodiscard]] uint32_t GetTileSetId() const noexcept
        {
            return this->_frames[this->_currentFrame].GetTileSetId();
        }

        [[nodiscard]] uint32_t GetIndexAbsolute() const noexcept
        {
            return this->_frames[this->_currentFrame].GetIndexAbsolute();
        }

        [[nodiscard]] uint64_t GetMilliseconds() const noexcept
        {
            return this->_frames[this->_currentFrame].GetMilliseconds();
        }

        [[nodiscard]] uint32_t GetTileSetId(uint32_t frame) const noexcept
        {
            return this->_frames[frame].GetTileSetId();
        }

        [[nodiscard]] uint32_t GetIndexAbsolute(uint32_t frame) const noexcept
        {
            return this->_frames[frame].GetIndexAbsolute();
        }

        [[nodiscard]] uint64_t GetMilliseconds(uint32_t frame) const noexcept
        {
            return this->_frames[frame].GetMilliseconds();
        }

        void SetFrame(uint32_t frameIndex, uint32_t tileSetId, uint32_t indexAbsolute, uint64_t milliseconds) noexcept
        {
            this->_frames[frameIndex].Set(tileSetId, indexAbsolute, milliseconds);
        }

        [[nodiscard]] bool IsIndexAbsoluteEmpty() const noexcept
        {
            return this->_frames[this->_currentFrame].GetIndexAbsolute() == TileMapTileAnimationData::IndexAbsoluteEmpty;
        }

        [[nodiscard]] uint32_t GetNumberOfFrames() const noexcept
        {
            return static_cast<uint32_t>(this->_frames.size());
        }

        void Tick(uint64_t frameTime);

        [[nodiscard]] bool operator== (const TileMapTile& other) const noexcept;

        [[nodiscard]] bool operator!= (const TileMapTile& other) const noexcept
        {
            return !(*this == other);
        }

        [[nodiscard]] bool operator== (const std::vector<TileMapTileAnimationData>& other) const noexcept;

        [[nodiscard]] bool operator!= (const std::vector<TileMapTileAnimationData>& other) const noexcept
        {
            return !(*this == other);
        }

    private:
        uint32_t _numberOfFrames {0}; // no need to call _frames.size() on each tick
        std::vector<TileMapTileAnimationData> _frames;
        uint32_t _currentFrame {0};
        uint64_t _currentAnimationTime {0};
    };
}

#endif
