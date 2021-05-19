#include <SDL.h>

#include "mouse_cursor.h"
#include "graphics/graphics.h"
#include "engine/mouse_input.h"

namespace projectfarm::graphics::ui
{
    bool MouseCursor::Create(const std::filesystem::path& path) noexcept
    {
        this->_texture = std::make_shared<Texture>();
        this->_texture->SetLogger(this->_logger);
        this->_texture->SetGraphics(this->GetGraphics());

        if (!this->_texture->Load(path))
        {
            this->LogMessage("Failed to load mouse cursor texture: " + path.u8string());
            return false;
        }

        this->_texture->SetRenderToWorldSpace(false);

        if (SDL_ShowCursor(SDL_DISABLE) < 0)
        {
            this->LogMessage("Failed hide cursor.");
            this->LogMessage(SDL_GetError());
            return false;
        }

        this->_canFocus = false;

        return true;
    }

    void MouseCursor::Clear()
    {
        this->ClearChildren();
    }

    void MouseCursor::Render()
    {
        if (!this->_texture)
        {
            return;
        }

        auto [x, y] = this->GetMouseInput()->GetPosition();

        this->_position.SetPositionFixed(x, y);

        this->_texture->SetRenderDetails(x, y, this->_size.GetWidth(), this->_size.GetHeight(),
                                                   graphics::RenderOriginPoints::TopLeft);

        auto renderLayer = this->GetGraphics()->BumpRenderLayer();
        this->_texture->Render(renderLayer);

        this->RenderChildren();
    }

    void MouseCursor::Shutdown() noexcept
    {
        this->_texture->Destroy();

        if (SDL_ShowCursor(SDL_ENABLE) < 0)
        {
            this->LogMessage("Failed show cursor.");
            this->LogMessage(SDL_GetError());
        }
    }

    bool MouseCursor::SetupFromJson(const nlohmann::json& controlJson,
                                    const std::shared_ptr<UI>& ui,
                                    const std::vector<std::pair<std::string, std::string>>& parameters)
    {
        auto normalizedJson = ui->NormalizeJson(controlJson, parameters);

        if (!this->SetCommonValuesFromJson(normalizedJson))
        {
            this->LogMessage("Failed to set common values.");
            return false;
        }

        auto texturePath = normalizedJson["texturePath"].get<std::string>();
        if (texturePath.empty())
        {
            this->LogMessage("No texture path provided for mouse cursor control.");
            return false;
        }

        auto path = this->_dataProvider->NormalizePath(texturePath);

        if (!this->Create(path))
        {
            this->LogMessage("Failed to create mouse cursor with texture from path: " + path.u8string());
            return false;
        }

        return true;
    }
}