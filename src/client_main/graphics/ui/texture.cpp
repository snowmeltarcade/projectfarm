#include "texture.h"
#include "graphics/graphics.h"
#include "graphics/shapes/shapes.h"

namespace projectfarm::graphics::ui
{
    bool Texture::Load() noexcept
    {
        this->_backgroundTexture = std::make_shared<projectfarm::graphics::Texture>();
        this->_backgroundTexture->SetLogger(this->_logger);
        this->_backgroundTexture->SetGraphics(this->GetGraphics());

        this->_backgroundTexture->SetRenderToWorldSpace(false);

        if (this->_style->Textures.empty())
        {
            this->_backgroundTexture->SetMaterialName("solid_color_with_tex_coords_with_mask");

            // we're not loading anything here, so set the size manually
            this->_backgroundTexture->SetTextureWidth(this->GetSize().GetWidth());
            this->_backgroundTexture->SetTextureHeight(this->GetSize().GetHeight());
        }
        else
        {
            auto texturePath = this->_style->Textures[0];
            if (!this->_backgroundTexture->Load(texturePath))
            {
                this->LogMessage("Failed to load texture: " + texturePath.u8string());
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

        this->SetColor(this->_style->Color);

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

        if (!this->Load())
        {
            this->LogMessage("Failed to load texture.");
            return false;
        }

        return true;
    }

    void Texture::ApplyStyle(bool isLoading) noexcept
    {
        // the texture is set when loading
        if (isLoading)
        {
            return;
        }

        this->_backgroundTexture->Destroy();
        this->_backgroundTexture = {};

        this->_maskTexture->Destroy();
        this->_maskTexture = {};

        if (!this->Load())
        {
            this->LogMessage("Failed to reload texture when applying style.");
        }
    }

    void Texture::OnDrag(uint32_t, uint32_t, uint32_t dx, uint32_t dy) noexcept
    {
        auto pos = this->GetPosition();

        auto [ox, oy] = pos.GetOffset();
        pos.SetOffset(ox + dx, oy + dy);

        this->SetPosition(pos);
    }
}
