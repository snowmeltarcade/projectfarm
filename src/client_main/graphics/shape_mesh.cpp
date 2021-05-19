#include "shape_mesh.h"
#include "graphics.h"

namespace projectfarm::graphics
{
    bool ShapeMesh::Load()
    {
        if (this->_loaded)
        {
            return false;
        }

        glGenVertexArrays(ShapeMesh::NumberOfBuffers, this->_vaoId);
        glGenBuffers(ShapeMesh::NumberOfBuffers, this->_vertexBOId);
        glGenBuffers(ShapeMesh::NumberOfBuffers, this->_indexBOId);

        for (auto i = 0u; i < ShapeMesh::NumberOfBuffers; ++i)
        {
            glBindVertexArray(this->_vaoId[i]);

            glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBOId[i]);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_indexBOId[i]);
        }

        glBindVertexArray(0);

        this->GetGraphics()->BindMaterial("solid_color");

        this->_loaded = true;

        return true;
    }

    void ShapeMesh::Destroy()
    {
        if (!this->_loaded)
        {
            return;
        }

        glDeleteVertexArrays(ShapeMesh::NumberOfBuffers, this->_vaoId);
        glDeleteBuffers(ShapeMesh::NumberOfBuffers, this->_vertexBOId);

        for (auto i = 0u; i < ShapeMesh::NumberOfBuffers; i++)
        {
            this->_vaoId[i] = 0;
            this->_vertexBOId[i] = 0;
        }

        this->_loaded = false;
    }

    uint32_t ShapeMesh::Render()
    {
        auto numberOfShapesRendered {0u};

        for (const auto&[_, renderLayer] : this->_meshMap)
        {
            for (const auto&[color, textureMap] : renderLayer)
            {
                const auto& verticies = textureMap._vertexData;
                const auto& indicies = textureMap._indexData;

                auto material = this->GetGraphics()->GetCurrentlyBoundMaterial();

                shared::graphics::colors::Color c;
                c.FromInt(color);
                material->SetColor(c);

                glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBOId[this->_currentBufferIndex]);
                glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(GLfloat), &verticies[0], GL_DYNAMIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_indexBOId[this->_currentBufferIndex]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLushort), &indicies[0], GL_DYNAMIC_DRAW);

                glBindVertexArray(this->_vaoId[this->_currentBufferIndex]);

                glDrawElements(GL_LINES, static_cast<GLsizei>(indicies.size()), GL_UNSIGNED_SHORT, nullptr);

                glBindVertexArray(0);

                ++numberOfShapesRendered;

                // double buffer the buffer objects
                this->_currentBufferIndex++;
                if (this->_currentBufferIndex >= NumberOfBuffers)
                {
                    this->_currentBufferIndex = 0;
                }
            }
        }

        return numberOfShapesRendered;
    }

    void ShapeMesh::ClearRender()
    {
        glBindVertexArray(0);
    }

    void ShapeMesh::AddShapeData(const shapes::Rectangle& shape,
                                 const SDL_Rect& screenSpace,
                                 uint32_t screenWidth, uint32_t screenHeight,
                                 uint32_t renderLayer) noexcept
    {
        // x1,y1    x2,y1
        // x1,y2    x2,y2

        // get the position data between 0..1
        GLfloat normalizedW = 1.0f / screenWidth * shape.GetW();
        GLfloat normalizedH = 1.0f / screenHeight * shape.GetH();

        GLfloat leftX = 1.0f / screenWidth * screenSpace.x;
        GLfloat topY = 1.0f - (1.0f / screenHeight * screenSpace.y);

        GLfloat rightX = leftX + normalizedW;
        GLfloat bottomY = topY - normalizedH;

        // get into screen space -1..1
        GLfloat ssLeftX = (leftX * 2.0f) - 1.0f;
        GLfloat ssRightX = (rightX * 2.0f) - 1.0f;

        GLfloat ssTopY = (topY * 2.0f) - 1.0f;
        GLfloat ssBottomY = (bottomY * 2.0f) - 1.0f;

        auto& meshMap = this->_meshMap[renderLayer][shape.GetColor().ToInt()];

        auto& verticiesMap = meshMap._vertexData;
        auto& indiciesMap = meshMap._indexData;

        verticiesMap.emplace_back(ssLeftX);
        verticiesMap.emplace_back(ssTopY);

        verticiesMap.emplace_back(ssLeftX);
        verticiesMap.emplace_back(ssBottomY);

        verticiesMap.emplace_back(ssLeftX);
        verticiesMap.emplace_back(ssBottomY);

        verticiesMap.emplace_back(ssRightX);
        verticiesMap.emplace_back(ssBottomY);

        verticiesMap.emplace_back(ssRightX);
        verticiesMap.emplace_back(ssBottomY);

        verticiesMap.emplace_back(ssRightX);
        verticiesMap.emplace_back(ssTopY);

        verticiesMap.emplace_back(ssRightX);
        verticiesMap.emplace_back(ssTopY);

        verticiesMap.emplace_back(ssLeftX);
        verticiesMap.emplace_back(ssTopY);

        auto size = static_cast<GLushort>(indiciesMap.size());
        indiciesMap.emplace_back(size++); // 0 + x
        indiciesMap.emplace_back(size++); // 1 + x
        indiciesMap.emplace_back(size++); // 2 + x
        indiciesMap.emplace_back(size++); // 3 + x
        indiciesMap.emplace_back(size++); // 4 + x
        indiciesMap.emplace_back(size++); // 5 + x
        indiciesMap.emplace_back(size++); // 6 + x
        indiciesMap.emplace_back(size); // 7 + x
    }

    void ShapeMesh::ClearShapeData() noexcept
    {
        // don't call `.clear()` on the maps as we want to keep and
        // reuse the allocated memory for the next frame
        for (auto&[_, layer] : this->_meshMap)
        {
            for (auto&[__, subLayer] : layer)
            {
                subLayer._vertexData.clear();
                subLayer._indexData.clear();
            }
        }
    }
}
