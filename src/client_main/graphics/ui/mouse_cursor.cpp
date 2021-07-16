#include <SDL.h>

#include "mouse_cursor.h"
#include "graphics/graphics.h"
#include "engine/mouse_input.h"
#include "api/logging/logging.h"

namespace projectfarm::graphics::ui
{
    bool MouseCursor::Create() noexcept
    {
        if (this->_style->Textures.empty())
        {
            // just use the system cursor
            return true;
        }

        this->_texture = std::make_shared<Texture>();
        this->_texture->SetGraphics(this->GetGraphics());

        auto path = this->_style->Textures[0];
        if (!this->_texture->Load(path))
        {
            shared::api::logging::Log("Failed to load mouse cursor texture: " + path.u8string());
            return false;
        }

        this->_texture->SetRenderToWorldSpace(false);

        if (SDL_ShowCursor(SDL_DISABLE) < 0)
        {
            shared::api::logging::Log("Failed hide cursor.");
            shared::api::logging::Log(SDL_GetError());
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
        if (!this->_texture)
        {
            return;
        }

        this->_texture->Destroy();

        if (SDL_ShowCursor(SDL_ENABLE) < 0)
        {
            shared::api::logging::Log("Failed show cursor.");
            shared::api::logging::Log(SDL_GetError());
        }
    }

    bool MouseCursor::SetupFromJson(const nlohmann::json& controlJson,
                                    const std::shared_ptr<UI>& ui,
                                    const std::vector<std::pair<std::string, std::string>>& parameters)
    {
        auto normalizedJson = ui->NormalizeJson(controlJson, parameters);

        if (!this->SetCommonValuesFromJson(normalizedJson))
        {
            shared::api::logging::Log("Failed to set common values.");
            return false;
        }

        if (!this->Create())
        {
            shared::api::logging::Log("Failed to create mouse cursor.");
            return false;
        }

        return true;
    }

    void MouseCursor::ApplyStyle(bool isLoading) noexcept
    {
        // the texture is set when loading
        if (isLoading)
        {
            return;
        }

        if (!this->Create())
        {
            shared::api::logging::Log("Failed to reload mouse cursor when applying style.");
        }
    }
}