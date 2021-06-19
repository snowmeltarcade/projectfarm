#include <SDL.h>

#include "graphics.h"
#include "opengl_errors.h"
#include "engine/debug_information.h"

namespace projectfarm::graphics
{
    bool Camera::SetSize(bool fullScreen, uint32_t width, uint32_t height) noexcept
    {
        this->_fullScreen = fullScreen;

        if (!this->_fullScreen)
        {
            SDL_SetWindowSize(this->GetGraphics()->GetWindow(), width, height);
        }

        int w {0};
        int h {0};
        SDL_GL_GetDrawableSize(this->GetGraphics()->GetWindow(), &w, &h);
        
        glViewport(0, 0, w, h);
        CHECK_OPENGL_ERROR

        // the screen size may not have changed in fullscreen mode (e.g. on iOS),
        // so just go with whatever it ended up as
        if (fullScreen) // TODO: this should check if we are HDPI, not fullscreen
        {
            width = w;
            height = h;
        }

        this->_viewport.w = width;
        this->_viewport.h = height;

        this->_logger->LogMessage("Drawable size: " + std::to_string(w) + "x" +
                                                         std::to_string(h));

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
}
