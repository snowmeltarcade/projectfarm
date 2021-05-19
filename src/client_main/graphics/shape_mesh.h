#ifndef PROJECTFARM_SHAPE_MESH_H
#define PROJECTFARM_SHAPE_MESH_H

#include <memory>
#include <vector>
#include <map>
#include <tuple>

#include "graphics_dependencies.h"
#include "logging/consume_logger.h"
#include "consume_graphics.h"
#include "shapes/shapes.h"

namespace projectfarm::graphics
{
    class ShapeMesh final : public shared::ConsumeLogger,
                            public ConsumeGraphics
    {
    public:
        ShapeMesh() = default;
        ~ShapeMesh() override = default;

        [[nodiscard]]
        bool Load();

        void Destroy();

        uint32_t Render();
        void ClearRender();

        void AddShapeData(const shapes::Rectangle& shape,
                          const SDL_Rect& screenSpace,
                          uint32_t screenWidth, uint32_t screenHeight,
                          uint32_t renderLayerIndex) noexcept;

        void ClearShapeData() noexcept;

    private:
        struct LayerDetails
        {
            std::vector<GLfloat> _vertexData;
            std::vector<GLushort> _indexData;
        };

        uint8_t _currentBufferIndex {0};
        const static uint8_t NumberOfBuffers {2};

        GLuint _vaoId[NumberOfBuffers] = {0};
        GLuint _vertexBOId[NumberOfBuffers] = {0};
        GLuint _indexBOId[NumberOfBuffers] = {0};

        std::map<uint32_t, std::map<uint32_t, LayerDetails>> _meshMap;

        bool _loaded {false};
    };
}

#endif
