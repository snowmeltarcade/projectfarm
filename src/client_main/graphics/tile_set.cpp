#include <nlohmann/json.hpp>
#include <fstream>

#include "tile_set.h"
#include "api/logging/logging.h"

namespace projectfarm::graphics
{
    bool TileSet::Load(const std::filesystem::path& filePath)
    {
        std::ifstream file(filePath);

        if (!file.is_open())
        {
            shared::api::logging::Log("Failed to open tileset file: " + filePath.u8string());
            return false;
        }

        nlohmann::json jsonFile;

        file >> jsonFile;

        auto name = jsonFile["name"].get<std::string>();
        auto imagePathString = jsonFile["imagePath"].get<std::string>();
        auto numberOfColumns = jsonFile["numberOfColumns"].get<uint32_t>();
        //auto numberOfTiles = jsonFile["numberOfTiles"].get<uint32_t>();
        auto tileWidth = jsonFile["tileWidth"].get<uint32_t>();
        auto tileHeight = jsonFile["tileHeight"].get<uint32_t>();

        std::filesystem::path imagePath = imagePathString;
        imagePath = this->_dataProvider->NormalizePath(imagePath);

        return this->LoadDirect(name, imagePath,
                                numberOfColumns,
                                tileWidth, tileHeight);
    }

    bool TileSet::LoadDirect(const std::string& name,
                             const std::filesystem::path& imagePath,
                             uint32_t numberOfColumns,
                             uint32_t tileWidth, uint32_t tileHeight)
    {
        this->_texture = std::make_shared<Texture>();

        this->_name = name;

        this->_numberOfColumns = numberOfColumns;
        this->_tileWidth = tileWidth;
        this->_tileHeight = tileHeight;

        this->_texture->SetLogger(this->_logger);
        this->_texture->SetGraphics(this->GetGraphics());

        if (!this->_texture->Load(imagePath))
        {
            shared::api::logging::Log("Failed to load tileset texture: " + imagePath.string());
            return false;
        }

        return true;
    }

    void TileSet::RenderTile(uint32_t index, int32_t x, int32_t y,
                             uint32_t targetTileWidth, uint32_t targetTileHeight,
                             bool renderToWorldSpace,
                             uint32_t renderLayerIndex)
    {
        this->_texture->SetRenderDetails(x, y,
                                         targetTileWidth, targetTileHeight,
                                         RenderOriginPoints::TopLeft);

        auto [tileX, tileY] = this->GetXYFromAbsoluteIndex(index);

        this->_texture->SetOriginDetails(tileX * this->_tileWidth, tileY * this->_tileHeight,
                                         this->_tileWidth, this->_tileHeight);

        this->_texture->SetRenderToWorldSpace(renderToWorldSpace);

        this->_texture->Render(renderLayerIndex);
    }

    void TileSet::Shutdown()
    {
        this->_texture->Destroy();
    }
}