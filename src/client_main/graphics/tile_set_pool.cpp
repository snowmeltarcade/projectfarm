#include <algorithm>

#include "tile_set_pool.h"
#include "api/logging/logging.h"

namespace projectfarm::graphics
{
    std::shared_ptr<TileSet> TileSetPool::Get(const std::string& name)
    {
        if (this->_items.count(name) == 0)
        {
            auto tileSet = std::make_shared<graphics::TileSet>();
            tileSet->SetLogger(this->_logger);
            tileSet->SetGraphics(this->GetGraphics());
            tileSet->SetDataProvider(this->_dataProvider);

            auto tileSetPath = this->_dataProvider->GetTileSetPathFromName(name);

            if (!tileSet->Load(tileSetPath))
            {
                shared::api::logging::Log("Failed to load tileset: " + tileSetPath.string());
                return {};
            }

            this->_items[name] = { tileSet, 0 };
        }

        this->_items[name].ReferenceCount++;
        return this->_items[name].TileSetData;
    }

    void TileSetPool::Release(const std::string& name)
    {
        auto iter = std::find_if(this->_items.begin(), this->_items.end(), [&name](const auto& it)
        {
           return it.first == name;
        });

        if (iter == this->_items.end())
        {
            return;
        }

        (*iter).second.ReferenceCount--;

        if ((*iter).second.ReferenceCount <= 0)
        {
            (*iter).second.TileSetData->Shutdown();
            this->_items.erase(iter);
        }
    }

    void TileSetPool::Empty()
    {
        this->_items.clear();
    }
}