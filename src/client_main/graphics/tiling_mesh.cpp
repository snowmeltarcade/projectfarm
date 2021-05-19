#include "tiling_mesh.h"
#include "graphics.h"

namespace projectfarm::graphics
{
    bool TilingMesh::Load()
    {
        if (this->_loaded)
        {
            return false;
        }

        glGenVertexArrays(TilingMesh::NumberOfBuffers, this->_vaoId);
        glGenBuffers(TilingMesh::NumberOfBuffers, this->_vertexBOId);
        glGenBuffers(TilingMesh::NumberOfBuffers, this->_textureBOId);
        glGenBuffers(TilingMesh::NumberOfBuffers, this->_indexBOId);

        for (auto i = 0u; i < TilingMesh::NumberOfBuffers; i++)
        {
            glBindVertexArray(this->_vaoId[i]);

            glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBOId[i]);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

            glBindBuffer(GL_ARRAY_BUFFER, this->_textureBOId[i]);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_indexBOId[i]);
        }

        glBindVertexArray(0);

        this->_loaded = true;

        return true;
    }

    void TilingMesh::Destroy()
    {
        if (!this->_loaded)
        {
            return;
        }

        glDeleteVertexArrays(TilingMesh::NumberOfBuffers, this->_vaoId);
        glDeleteBuffers(TilingMesh::NumberOfBuffers, this->_vertexBOId);
        glDeleteBuffers(TilingMesh::NumberOfBuffers, this->_textureBOId);

        for (auto i = 0u; i < TilingMesh::NumberOfBuffers; i++)
        {
            this->_vaoId[i] = 0;
            this->_vertexBOId[i] = 0;
            this->_textureBOId[i] = 0;
        }

        this->_loaded = false;
    }

    uint32_t TilingMesh::Render()
    {
        auto numberOfMeshesRendered {0u};

        for (const auto&[_, renderLayer] : this->_meshMap)
        {
            for (const auto&[__, textureMap] : renderLayer)
            {
                if (!textureMap._texture)
                {
                    // this must be a buffered element not used in this frame
                    continue;
                }

                const auto& verticies = textureMap._vertexData;
                const auto& textureCoordinates = textureMap._textureCoordinateData;
                const auto& indicies = textureMap._indexData;
                const auto& texture = textureMap._texture;

                auto materialName = texture->GetMaterialName();
                this->GetGraphics()->BindMaterial(materialName);

                auto material = this->GetGraphics()->GetCurrentlyBoundMaterial();
                material->SetColor(texture->GetColor());

                glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBOId[this->_currentBufferIndex]);
                glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(GLfloat), &verticies[0], GL_DYNAMIC_DRAW);

                glBindBuffer(GL_ARRAY_BUFFER, this->_textureBOId[this->_currentBufferIndex]);
                glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof(GLfloat), &textureCoordinates[0], GL_DYNAMIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_indexBOId[this->_currentBufferIndex]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLushort), &indicies[0], GL_DYNAMIC_DRAW);

                texture->Bind();

                glBindVertexArray(this->_vaoId[this->_currentBufferIndex]);

                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicies.size()), GL_UNSIGNED_SHORT, nullptr);

                glBindVertexArray(0);

                ++numberOfMeshesRendered;

                // double buffer the buffer objects
                this->_currentBufferIndex++;
                if (this->_currentBufferIndex >= NumberOfBuffers)
                {
                    this->_currentBufferIndex = 0;
                }
            }
        }

        return numberOfMeshesRendered;
    }

    void TilingMesh::ClearRender()
    {
        glBindVertexArray(0);
    }

    void TilingMesh::AddTileData(const std::shared_ptr<Texture>& texture,
                                 float sx, float sy, float sw, float sh,
                                 float dx, float dy, float dw, float dh,
                                 uint32_t screenWidth, uint32_t screenHeight,
                                 uint32_t renderLayer)
    {
        if (!texture)
        {
            this->LogMessage("Null texture.");
            return;
        }

        // -1  1    1  1
        // -1 -1    1 -1

        // get the position data between 0..1
        GLfloat normalizedW = 1.0f / screenWidth * dw;
        GLfloat normalizedH = 1.0f / screenHeight * dh;

        GLfloat leftX = 1.0f / screenWidth * dx;
        GLfloat topY = 1.0f - (1.0f / screenHeight * dy);

        GLfloat rightX = leftX + normalizedW;
        GLfloat bottomY = topY - normalizedH;

        // get into screen space -1..1
        GLfloat ssLeftX = (leftX * 2.0f) - 1.0f;
        GLfloat ssRightX = (rightX * 2.0f) - 1.0f;

        GLfloat ssTopY = (topY * 2.0f) - 1.0f;
        GLfloat ssBottomY = (bottomY * 2.0f) - 1.0f;

        // normalize the texture coords
        GLfloat normalizedTextureX = sx / texture->GetTextureWidth();
        GLfloat normalizedTextureY = sy / texture->GetTextureHeight();

        GLfloat normalizedTextureW = sw / texture->GetTextureWidth();
        GLfloat normalizedTextureH = sh / texture->GetTextureHeight();

        GLfloat normalizedTextureX2 = normalizedTextureX + normalizedTextureW;
        GLfloat normalizedTextureY2 = normalizedTextureY + normalizedTextureH;

        GLuint textureId = texture->GetTextureId();

        // if textureId == 0, then we are likely rendering a solid color
        auto& meshMap = this->_meshMap[renderLayer][textureId];

        auto& verticiesMap = meshMap._vertexData;
        auto& textureCoordinatesMap = meshMap._textureCoordinateData;
        auto& indiciesMap = meshMap._indexData;

        meshMap._texture = texture;

        verticiesMap.emplace_back(ssLeftX); //CCW
        verticiesMap.emplace_back(ssTopY);

        verticiesMap.emplace_back(ssLeftX);
        verticiesMap.emplace_back(ssBottomY);

        verticiesMap.emplace_back(ssRightX);
        verticiesMap.emplace_back(ssTopY);

        verticiesMap.emplace_back(ssRightX);
        verticiesMap.emplace_back(ssTopY);

        verticiesMap.emplace_back(ssLeftX);
        verticiesMap.emplace_back(ssBottomY);

        verticiesMap.emplace_back(ssRightX);
        verticiesMap.emplace_back(ssBottomY);

        textureCoordinatesMap.emplace_back(normalizedTextureX);
        textureCoordinatesMap.emplace_back(normalizedTextureY);

        textureCoordinatesMap.emplace_back(normalizedTextureX);
        textureCoordinatesMap.emplace_back(normalizedTextureY2);

        textureCoordinatesMap.emplace_back(normalizedTextureX2);
        textureCoordinatesMap.emplace_back(normalizedTextureY);

        textureCoordinatesMap.emplace_back(normalizedTextureX2);
        textureCoordinatesMap.emplace_back(normalizedTextureY);

        textureCoordinatesMap.emplace_back(normalizedTextureX);
        textureCoordinatesMap.emplace_back(normalizedTextureY2);

        textureCoordinatesMap.emplace_back(normalizedTextureX2);
        textureCoordinatesMap.emplace_back(normalizedTextureY2);

        auto size = static_cast<GLushort>(indiciesMap.size());
        indiciesMap.emplace_back(size++); // 0 + x
        indiciesMap.emplace_back(size++); // 1 + x
        indiciesMap.emplace_back(size++); // 2 + x
        indiciesMap.emplace_back(size++); // 3 + x
        indiciesMap.emplace_back(size++); // 4 + x
        indiciesMap.emplace_back(size); // 5 + x
    }

    void TilingMesh::ClearTileData()
    {
        // don't call `.clear()` on the maps as we want to keep and
        // reuse the allocated memory for the next frame
        for (auto&[_, layer] : this->_meshMap)
        {
            for (auto&[__, subLayer] : layer)
            {
                subLayer._texture = nullptr;
                subLayer._vertexData.clear();
                subLayer._textureCoordinateData.clear();
                subLayer._indexData.clear();
            }
        }
    }
}
