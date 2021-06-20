#ifndef PROJECTFARM_SCREEN_RESOLUTION_H
#define PROJECTFARM_SCREEN_RESOLUTION_H

#include <cstdint>
#include <string>

#include "platform/platform_id.h"

using namespace std::literals;

namespace projectfarm::graphics
{
    struct ScreenResolution
    {
        ScreenResolution(uint32_t width, uint32_t height)
                : Width(width), Height(height)
        {}

        ScreenResolution(uint32_t width, uint32_t height, bool isDefault)
            : Width(width), Height(height), Default(isDefault)
        {}

        uint32_t Width {0};
        uint32_t Height {0};
#ifdef IS_IOS
        // iOS is always fullscreen, so make the default the same
        bool FullScreen {true};
#else
        bool FullScreen {false};
#endif
        bool Default {false};

        std::string GetName() const noexcept
        {
            return (this->FullScreen ? "Full Screen" : "Windowed") + " "s +
                   std::to_string(this->Width) + "x" + std::to_string(this->Height);
        }
    };
}

#endif
