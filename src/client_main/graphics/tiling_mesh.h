#ifndef PROJECTFARM_TILING_MESH_H
#define PROJECTFARM_TILING_MESH_H

#include <memory>
#include <vector>
#include <map>
#include <tuple>

#include "graphics_dependencies.h"
#include "texture.h"
#include "consume_graphics.h"

namespace projectfarm::graphics
{
    class TilingMesh final : public ConsumeGraphics
    {
    public:
        TilingMesh() = default;
        ~TilingMesh() override = default;

        [[nodiscard]] bool Load();
        void Destroy();

        uint32_t Render();
        void ClearRender();

        void AddTileData(const std::shared_ptr<Texture>& texture,
                         float sx, float sy, float sw, float sh,
                         float dx, float dy, float dw, float dh,
                         uint32_t screenWidth, uint32_t screenHeight,
                         uint32_t renderLayerIndex);

        void ClearTileData();

    private:
        struct LayerDetails
        {
            std::shared_ptr<Texture> _texture;
            std::vector<GLfloat> _vertexData;
            std::vector<GLfloat> _textureCoordinateData;
            std::vector<GLushort> _indexData;
        };

        uint8_t _currentBufferIndex {0};
        const static uint8_t NumberOfBuffers {2};

        GLuint _vaoId[NumberOfBuffers] = {0};
        GLuint _vertexBOId[NumberOfBuffers] = {0};
        GLuint _textureBOId[NumberOfBuffers] = {0};
        GLuint _indexBOId[NumberOfBuffers] = {0};

        std::map<uint32_t, std::map<uint32_t, LayerDetails>> _meshMap;

        bool _loaded {false};
    };
}

#endif
