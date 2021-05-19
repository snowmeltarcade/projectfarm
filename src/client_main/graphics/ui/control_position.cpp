#include "control_position.h"

#include "base_control.h"

namespace projectfarm::graphics::ui
{
    void ControlPosition::ResolvePosition(const std::shared_ptr<const BaseControl>& control,
                                          uint32_t& outX, uint32_t& outY) const noexcept
    {
        if (control == nullptr)
        {
            outX = 0;
            outY = 0;
            return;
        }

        auto parent = control->GetParent();

        if (!parent)
        {
            outX = this->_x;
            outY = this->_y;
            return;
        }

        auto parentPos = parent->GetPosition();
        uint32_t parentX {0};
        uint32_t parentY {0};
        parentPos.ResolvePosition(parent, parentX, parentY);

        auto [parentOffsetX, parentOffsetY] = parentPos.GetOffset();

        if (this->_percentX > 0.0f || this->_percentY > 0.0f)
        {
            auto size = parent->GetSize();
            auto w = size.GetWidth();
            auto h = size.GetHeight();

            auto x = (w / 100.0f) * this->_percentX;
            auto y = (h / 100.0f) * this->_percentY;

            outX = parentX + parentOffsetX + static_cast<uint32_t>(x);
            outY = parentY + parentOffsetY + static_cast<uint32_t>(y);
        }
        else
        {
            outX = parentX + parentOffsetX + this->_x;
            outY = parentY + parentOffsetY + this->_y;
        }

        if (this->_renderOriginPoint == projectfarm::graphics::RenderOriginPoints::Center)
        {
            auto size = control->GetSize();
            outX -= size.GetWidth() / 2;
            outY -= size.GetHeight() / 2;
        }
    }
}