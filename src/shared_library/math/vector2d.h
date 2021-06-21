#ifndef PROJECTFARM_VECTOR2D_H
#define PROJECTFARM_VECTOR2D_H

#include <cstdint>

#include "scripting/gc_persistent.h"

namespace projectfarm::shared::math
{
    class Vector2D
    {
    public:
        [[nodiscard]]
        int32_t GetX() const noexcept
        {
            return this->_x;
        }

        void SetX(int32_t x) noexcept
        {
            this->_x = x;
        }

        [[nodiscard]]
        int32_t GetY() const noexcept
        {
            return this->_y;
        }

        void SetY(int32_t y) noexcept
        {
            this->_y = y;
        }

    private:
        int32_t _x {0};
        int32_t _y {0};
    };

    // We don't want Vector2D to carry v8 baggage, so use this if you need to
    // use Vector2D and need to free memory allocated for v8 to use it
    class ScriptableVector2D : public shared::scripting::GCPersistent
    {
    public:
        ScriptableVector2D() = default;
        ~ScriptableVector2D() override = default;

        Vector2D _object;
    };
}

#endif
