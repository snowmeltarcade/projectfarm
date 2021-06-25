#include "texture.h"
#include "graphics/graphics.h"
#include "graphics/shapes/shapes.h"

namespace projectfarm::graphics::ui
{
    bool Texture::Load(const std::filesystem::path& texturePath) noexcept
    {
        this->_backgroundTexture = std::make_shared<projectfarm::graphics::Texture>();
        this->_backgroundTexture->SetLogger(this->_logger);
        this->_backgroundTexture->SetGraphics(this->GetGraphics());

        this->_backgroundTexture->SetRenderToWorldSpace(false);

        if (texturePath.empty())
        {
            this->_backgroundTexture->SetMaterialName("solid_color_with_tex_coords_with_mask");

            // we're not loading anything here, so set the size manually
            this->_backgroundTexture->SetTextureWidth(this->GetSize().GetWidth());
            this->_backgroundTexture->SetTextureHeight(this->GetSize().GetHeight());
        }
        else
        {
            if (!this->_backgroundTexture->Load(texturePath))
            {
                this->LogMessage("Failed to load texture: " + texturePath.string());
                return false;
            }

            this->_backgroundTexture->SetMaterialName("single_texture_with_mask");
        }

        if (!this->CreateMask())
        {
            this->LogMessage("Failed to create mask.");
            return false;
        }

        this->_backgroundTexture->AddMaterialTexture(this->_maskTexture);

        this->SetColor(shared::graphics::colors::White);

        return true;
    }

    void Texture::Clear()
    {
        if (this->_backgroundTexture)
        {
            this->_backgroundTexture->Destroy();
            this->_backgroundTexture = nullptr;
        }

        this->ClearChildren();
    }

    void Texture::Render()
    {
        if (this->_backgroundTexture)
        {
            uint32_t x {0};
            uint32_t y {0};

            this->_position.ResolvePosition(this->shared_from_this(), x, y);

            auto [offsetX, offsetY] = this->_position.GetOffset();

            this->_backgroundTexture->SetRenderDetails(x + offsetX, y + offsetY,
                                                       this->_size.GetWidth(), this->_size.GetHeight(),
                                                       graphics::RenderOriginPoints::TopLeft);

            this->_backgroundTexture->ClearMaterialTextures();
            this->_backgroundTexture->AddMaterialTexture(this->_maskTexture);

            auto renderLayer = this->GetGraphics()->BumpRenderLayer();
            this->_backgroundTexture->Render(renderLayer);
        }

        this->RenderChildren();
    }

    bool Texture::SetupFromJson(const nlohmann::json& controlJson,
                                const std::shared_ptr<UI>& ui,
                                const std::vector<std::pair<std::string, std::string>>& parameters)
    {
        auto normalizedJson = ui->NormalizeJson(controlJson, parameters);

        if (!this->SetCommonValuesFromJson(normalizedJson))
        {
            this->LogMessage("Failed to set common values.");
            return false;
        }

        // if neither color of texture path is defined, this texture will default to white
        auto colorNameIter = normalizedJson.find("color");
        if (colorNameIter != normalizedJson.end())
        {
            auto colorName = colorNameIter->get<std::string>();
            if (!colorName.empty())
            {
                if (auto color = shared::graphics::colors::FromString(colorName); color)
                {
                    this->SetColor(*color);
                }
            }
        }

        std::filesystem::path path;

        auto texturePathIter = normalizedJson.find("texturePath");
        if (texturePathIter != normalizedJson.end())
        {
            auto texturePath = texturePathIter->get<std::string>();
            if (texturePath.empty())
            {
                return true;
            }

            path = this->_dataProvider->NormalizePath(texturePath);
        }

        // if the path is empty, the texture will just have a color
        if (!this->Load(path))
        {
            this->LogMessage("Failed to load texture from path: " + path.u8string());
            return false;
        }

        return true;
    }

    void Texture::OnDrag(uint32_t, uint32_t, uint32_t dx, uint32_t dy) noexcept
    {
        auto pos = this->GetPosition();

        auto [ox, oy] = pos.GetOffset();
        pos.SetOffset(ox + dx, oy + dy);

        this->SetPosition(pos);
    }
}
