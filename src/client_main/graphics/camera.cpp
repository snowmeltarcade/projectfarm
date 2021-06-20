#include <SDL.h>

#include "graphics.h"
#include "opengl_errors.h"
#include "engine/debug_information.h"

namespace projectfarm::graphics
{
    bool Camera::SetResolution(const ScreenResolution& screenResolution) noexcept
    {
        this->_currentScreenResolution = screenResolution;

        this->_logger->LogMessage("Setting screen resolution to: " + screenResolution.GetName());

        if (this->_currentScreenResolution->FullScreen)
        {
            SDL_DisplayMode displayMode
            {
                SDL_PIXELFORMAT_RGBA32,
                static_cast<int>(this->_currentScreenResolution->Width),
                static_cast<int>(this->_currentScreenResolution->Height),
                0,
                0,
            };

            SDL_SetWindowDisplayMode(this->GetGraphics()->GetWindow(), &displayMode);

            SDL_SetWindowFullscreen(this->GetGraphics()->GetWindow(), SDL_WINDOW_FULLSCREEN);
        }
        else
        {
            SDL_SetWindowSize(this->GetGraphics()->GetWindow(),
                              this->_currentScreenResolution->Width, this->_currentScreenResolution->Height);

            SDL_SetWindowFullscreen(this->GetGraphics()->GetWindow(), 0);
        }

        int w {0};
        int h {0};
        SDL_GL_GetDrawableSize(this->GetGraphics()->GetWindow(), &w, &h);
        
        glViewport(0, 0, w, h);
        CHECK_OPENGL_ERROR

        this->_viewport.w = w;
        this->_viewport.h = h;

        this->_logger->LogMessage("Viewport size: " + std::to_string(this->_viewport.w) + "x" +
                                  std::to_string(this->_viewport.h));

        this->_pixelsPerMeter = this->_viewport.w / this->_screenWidthInMeters;

        this->_logger->LogMessage("Pixels/Meter: " + std::to_string(this->_pixelsPerMeter));

        return true;
    }

    bool Camera::RenderTexture(const std::shared_ptr<Texture>& texture,
                               SDL_Rect* source, SDL_Rect* dest,
                               bool isDestWorldSpace,
                               uint32_t renderLayerIndex)
    {
        SDL_Rect screenSpace {0,0,0,0};

        if (dest)
        {
            if (isDestWorldSpace)
            {
                if (!SDL_HasIntersection(&this->_viewport, dest))
                {
                    return true;
                }

                screenSpace = this->WorldSpaceToScreenSpace(*dest);
            }
            else
            {
                screenSpace = *dest;
            }
        }

        SDL_Rect* renderDest = dest == nullptr ? nullptr : &screenSpace; // nullptr == fullscreen

        this->GetGraphics()->PushTileToRender(texture,
                                              source, renderDest,
                                              renderLayerIndex);

        this->GetDebugInformation()->AddDrawnTile();

        return true;
    }

    void Camera::RenderShape(const graphics::shapes::Rectangle& shape,
                             bool isDestWorldSpace,
                             uint32_t renderLayerIndex) noexcept
    {
        SDL_Rect screenSpace {0,0,0,0};

        auto dest = shape.GetBoundingRect();

        if (isDestWorldSpace)
        {
            if (!SDL_HasIntersection(&this->_viewport, &dest))
            {
                return;
            }

            screenSpace = this->WorldSpaceToScreenSpace(dest);
        }
        else
        {
            screenSpace = dest;
        }

        this->GetGraphics()->PushShapeToRender(shape, screenSpace,
                                               renderLayerIndex);

        // TODO: AddDrawnShape
        this->GetDebugInformation()->AddDrawnTile();
    }

    void Camera::ProcessAction(engine::ActionTypes action, bool actionAdded) noexcept
    {
        if (!actionAdded)
        {
            return;
        }

        if (action == engine::ActionTypes::MoveUp)
        {
            this->_viewport.y++;
        }
        else if (action == engine::ActionTypes::MoveDown)
        {
            this->_viewport.y--;
        }
        else if (action == engine::ActionTypes::MoveRight)
        {
            this->_viewport.x++;
        }
        else if (action == engine::ActionTypes::MoveLeft)
        {
            this->_viewport.x--;
        }
    }

    SDL_Rect Camera::WorldSpaceToScreenSpace(SDL_Rect worldSpace) const noexcept
    {
        SDL_Rect screenSpace {
            worldSpace.x - this->_viewport.x,
            worldSpace.y - this->_viewport.y,
            worldSpace.w,
            worldSpace.h,
        };

        return screenSpace;
    }

    std::optional<ScreenResolution> Camera::GetCurrentResolution() const noexcept
    {
        if (this->_currentScreenResolution)
        {
            return this->_currentScreenResolution;
        }

        auto defaultResolution = std::find_if(this->_defaultScreenResolutions.begin(),
                                              this->_defaultScreenResolutions.end(),
                                              [](const auto& res) { return res.Default; });

        if (defaultResolution == this->_defaultScreenResolutions.end())
        {
            this->LogMessage("Failed to find default resolution.");
            return {};
        }

        return *defaultResolution;
    }
}
