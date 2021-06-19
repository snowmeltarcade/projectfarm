#ifndef PROJECTFARM_CAMERA_H
#define PROJECTFARM_CAMERA_H

#include <cstdlib>
#include <string>

#include "logging/consume_logger.h"
#include "graphics/consume_graphics.h"
#include "engine/actionable_entity.h"
#include "engine/consume_debug_information.h"
#include "texture.h"
#include "engine/action_types.h"
#include "graphics/shapes/shapes.h"

namespace projectfarm::graphics
{
    class Camera final : public shared::ConsumeLogger,
                         public graphics::ConsumeGraphics,
                         public engine::ActionableEntity,
                         public engine::ConsumeDebugInformation
    {
    public:
        Camera() = default;
        ~Camera() override = default;

        void SetPosition(int32_t x, int32_t y) noexcept
        {
            this->_viewport.x = x;
            this->_viewport.y = y;
        }

        void SetPosition(float x, float y) noexcept
        {
            this->_viewport.x = static_cast<int32_t>(x);
            this->_viewport.y = static_cast<int32_t>(y);
        }

        [[nodiscard]]
        bool SetSize(uint32_t width, uint32_t height) noexcept
        {
            return this->SetSize(this->GetFullScreen(), width, height);
        }

        [[nodiscard]]
        bool SetSize(bool fullScreen = false, uint32_t width = 800, uint32_t height = 600) noexcept;

        [[nodiscard]]
        bool RenderTexture(const std::shared_ptr<Texture>& texture,
                           SDL_Rect* source, SDL_Rect* dest,
                           bool isDestWorldSpace = true,
                           uint32_t renderLayerIndex = 0);

        void RenderShape(const graphics::shapes::Rectangle& shape,
                         bool isDestWorldSpace = true,
                         uint32_t renderLayerIndex = 0) noexcept;

        [[nodiscard]]
        const SDL_Rect& GetViewport() const
        {
            return this->_viewport;
        }

        void ProcessAction(engine::ActionTypes action, bool actionAdded) noexcept override;

        [[nodiscard]]
        SDL_Rect WorldSpaceToScreenSpace(SDL_Rect worldSpace) const noexcept;

        void SetScreenWidthInMeters(uint32_t screenWidthInMeters) noexcept
        {
            this->_screenWidthInMeters = screenWidthInMeters;
        }

        [[nodiscard]]
        uint32_t GetPixelsPerMeter() const noexcept
        {
            return this->_pixelsPerMeter;
        }

        [[nodiscard]]
        bool GetFullScreen() const noexcept
        {
            return this->_fullScreen;
        }

        [[nodiscard]]
        // this assumes x and y are in the range of 0.0f...1.0f
        std::pair<uint32_t, uint32_t> GetPositionAtPercent(float x, float y) const noexcept
        {
            return { static_cast<uint32_t>(static_cast<float>(this->_viewport.w) * x),
                     static_cast<uint32_t>(static_cast<float>(this->_viewport.h) * y) };
        }

    private:
        SDL_Rect _viewport {};

        uint32_t _screenWidthInMeters {10};
        uint32_t _pixelsPerMeter {1};

        bool _fullScreen {false};
    };
}

#endif
