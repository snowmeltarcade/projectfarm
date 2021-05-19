#ifndef PROJECTFARM_RECTANGLE_H
#define PROJECTFARM_RECTANGLE_H

#include "graphics/colors/color.h"
#include "graphics/renderable.h"

namespace projectfarm::graphics::shapes
{
    class Rectangle
    {
    public:
        Rectangle() = default;
        Rectangle(int32_t x, int32_t y, uint32_t w, uint32_t h, shared::graphics::colors::Color color)
        : _x(x), _y(y), _w(w), _h(h), _color(std::move(color))
        {}
        ~Rectangle() = default;

        [[nodiscard]]
        const shared::graphics::colors::Color& GetColor() const noexcept
        {
            return this->_color;
        }

        [[nodiscard]]
        int32_t GetX() const noexcept
        {
            return this->_x;
        }

        [[nodiscard]]
        int32_t GetY() const noexcept
        {
            return this->_y;
        }

        [[nodiscard]]
        uint32_t GetW() const noexcept
        {
            return this->_w;
        }

        [[nodiscard]]
        uint32_t GetH() const noexcept
        {
            return this->_h;
        }

        [[nodiscard]]
        SDL_Rect GetBoundingRect() const noexcept
        {
            return { this->_x,
                     this->_y,
                     static_cast<int>(this->_w),
                     static_cast<int>(this->_h) };
        }

    private:
        int32_t _x {0};
        int32_t _y {0};
        uint32_t _w {0};
        uint32_t _h {0};

        shared::graphics::colors::Color _color;
    };
}

#endif
