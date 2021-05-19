#ifndef PROJECTFARM_TILE_SET_POOL_H
#define PROJECTFARM_TILE_SET_POOL_H

#include <memory>
#include <string>
#include <unordered_map>

#include "tile_set_pool_data.h"
#include "consume_graphics.h"
#include "logging/consume_logger.h"
#include "data/consume_data_provider.h"

namespace projectfarm::graphics
{
    // this pool has no cleanup methods as it is unlikely that tilesets will need to
    // hang around, in case they are reloaded, like textures do
    class TileSetPool final : public shared::ConsumeLogger,
                              public ConsumeGraphics,
                              public shared::ConsumeDataProvider
    {
    public:
        TileSetPool() = default;
        ~TileSetPool() override = default;

        [[nodiscard]] std::shared_ptr<TileSet> Get(const std::string& name);

        void Release(const std::string& name);

        void Empty();

    private:
        std::unordered_map<std::string, TileSetPoolData> _items;
    };
}

#endif
