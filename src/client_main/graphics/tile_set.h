#ifndef PROJECTFARM_TILE_SET_H
#define PROJECTFARM_TILE_SET_H

#include <filesystem>
#include <tuple>

#include "data/consume_data_provider.h"
#include "consume_graphics.h"
#include "texture.h"

namespace projectfarm::graphics
{
    class TileSet final : public ConsumeGraphics,
                          public shared::ConsumeDataProvider
    {
    public:
        TileSet() = default;
        ~TileSet() override = default;

        [[nodiscard]] bool Load(const std::filesystem::path& filePath);
        [[nodiscard]] bool LoadDirect(const std::string& name,
                                      const std::filesystem::path& imagePath,
                                      uint32_t numberOfColumns,
                                      uint32_t tileWidth, uint32_t tileHeight);

        void RenderTile(uint32_t index, int32_t x, int32_t y,
                        uint32_t targetTileWidth, uint32_t targetTileHeight,
                        bool renderToWorldSpace,
                        uint32_t renderLayerIndex);

        [[nodiscard]] std::string GetName() const noexcept
        {
            return this->_name;
        }

        [[nodiscard]] uint32_t GetAbsoluteIndex(uint32_t x, uint32_t y) const noexcept
        {
            return x + (y * this->_numberOfColumns);
        }

        [[nodiscard]] auto GetXYFromAbsoluteIndex(uint32_t absoluteIndex) const noexcept
        {
            auto tileX = (uint32_t)(absoluteIndex % this->_numberOfColumns);
            auto tileY = (uint32_t)(absoluteIndex / this->_numberOfColumns);

            return std::make_pair(tileX, tileY);
        }

        [[nodiscard]] bool IsAbsoluteIndexValid(uint32_t absoluteIndex) const noexcept
        {
            auto [x, y] = this->GetXYFromAbsoluteIndex(absoluteIndex);

            return x < this->_numberOfColumns &&
                   y < (this->_texture->GetTextureHeight() / this->_tileHeight);
        }

    private:
        std::shared_ptr<Texture> _texture;

        std::string _name;

        uint32_t _numberOfColumns {0};
        uint32_t _tileWidth {0};
        uint32_t _tileHeight {0};

        void Shutdown();

        friend class TileSetPool;
    };
}

#endif
