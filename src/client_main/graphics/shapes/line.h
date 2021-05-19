#ifndef PROJECTFARM_LINE_H
#define PROJECTFARM_LINE_H

#include <cstdint>
#include <vector>
#include <SDL.h>

namespace projectfarm::graphics::shapes
{
    class Line
    {
    public:
        void Set(int32_t x1, int32_t y1, int32_t x2, int32_t y2) noexcept
        {
            this->_x1 = x1;
            this->_y1 = y1;
            this->_x2 = x2;
            this->_y2 = y2;
        }

    private:
        int32_t _x1 {0};
        int32_t _y1 {0};

        int32_t _x2 {0};
        int32_t _y2 {0};
    };
}

#endif
