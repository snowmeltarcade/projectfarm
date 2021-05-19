#ifndef PROJECTFARM_CONTROL_POSITION_H
#define PROJECTFARM_CONTROL_POSITION_H

#include <cstdlib>
#include <memory>

#include "graphics/render_origin_points.h"

namespace projectfarm::graphics::ui
{
    class BaseControl;

    class ControlPosition final
    {
    public:
        ControlPosition() = default;
        ~ControlPosition() = default;

        void SetPositionFixed(uint32_t x, uint32_t y) noexcept
        {
            this->_x = x;
            this->_y = y;

            this->_percentX = 0.0f;
            this->_percentY = 0.0f;
        }

        void SetPositionPercent(float x, float y) noexcept
        {
            this->_percentX = x;
            this->_percentY = y;

            this->_x = 0;
            this->_y = 0;
        }

        [[nodiscard]] projectfarm::graphics::RenderOriginPoints GetRenderOriginPoint() const noexcept
        {
            return this->_renderOriginPoint;
        }

        void SetRenderOriginPoint(projectfarm::graphics::RenderOriginPoints renderOriginPoint) noexcept
        {
            this->_renderOriginPoint = renderOriginPoint;
        }

        void ResolvePosition(const std::shared_ptr<const BaseControl>& control,
                             uint32_t& outX, uint32_t& outY) const noexcept;

        [[nodiscard]]
        std::pair<uint32_t, uint32_t> GetPosition() const noexcept
        {
            return { this->_x, this->_y };
        }

        [[nodiscard]]
        std::pair<int32_t, int32_t> GetOffset() const noexcept
        {
            return { this->_offsetX, this->_offsetY };
        }

        void SetOffset(int32_t offsetX, int32_t offsetY) noexcept
        {
            this->_offsetX = offsetX;
            this->_offsetY = offsetY;
        }

    private:
        uint32_t _x {0};
        uint32_t _y {0};

        int32_t _offsetX {0};
        int32_t _offsetY {0};

        float _percentX {0.0f};
        float _percentY {0.0f};

        RenderOriginPoints _renderOriginPoint {RenderOriginPoints::TopLeft};
    };
}

#endif
