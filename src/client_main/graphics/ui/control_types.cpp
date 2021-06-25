#include "control_types.h"

namespace projectfarm::graphics::ui
{
    std::string ControlTypesToString(ControlTypes type) noexcept
    {
        switch (type)
        {
            case ControlTypes::Custom:
            {
                return "custom";
            }
            case ControlTypes::Canvas:
            {
                return "canvas";
            }
            case ControlTypes::Label:
            {
                return "label";
            }
            case ControlTypes::MouseCursor:
            {
                return "mouseCursor";
            }
            case ControlTypes::Texture:
            {
                return "texture";
            }
            default:
            {
                return "unknown";
            }
        }
    }
}