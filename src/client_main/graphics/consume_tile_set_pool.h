#ifndef CONSUME_TILE_SET_POOL_H
#define CONSUME_TILE_SET_POOL_H

#include <memory>

#include "graphics/tile_set_pool.h"

namespace projectfarm::graphics
{
	class ConsumeTileSetPool
	{
	public:
        ConsumeTileSetPool() = default;
		virtual ~ConsumeTileSetPool() = default;

		[[nodiscard]] const std::shared_ptr<TileSetPool>& GetTileSetPool() const
		{
			return _tileSetPool;
		}

		void SetTileSetPool(const std::shared_ptr<TileSetPool>& tileSetPool)
		{
			this->_tileSetPool = tileSetPool;
		}

	protected:
		std::shared_ptr<TileSetPool> _tileSetPool;
	};
}

#endif