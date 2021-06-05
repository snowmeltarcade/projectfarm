#ifndef PROJECTFARM_DEVICE_CAPABILITIES_H
#define PROJECTFARM_DEVICE_CAPABILITIES_H

#include <SDL.h>

#include "platform.h"

namespace projectfarm::engine
{
    struct DeviceCapabilities
    {
        // make an educated guess as these caps will be set
        // during runtime
        // TODO: Add Android define here when we support it one day
#ifdef IS_IOS
        bool HasTouchScreen = true;
        bool HasPhysicalKeyboard = false;
        bool HasOnScreenKeyboard = true;
        bool HasPhysicalMouse = false;
        bool HasGameController = false;
        bool HasJoystick = false;
#else
        bool HasTouchScreen = false;
        bool HasPhysicalKeyboard = true;
        bool HasOnScreenKeyboard = false;
        bool HasPhysicalMouse = true;
        bool HasGameController = false;
        bool HasJoystick = false;
#endif
    };

    [[nodiscard]]
    DeviceCapabilities GetDeviceCapabilities() noexcept;

    void UpdateDeviceCapabilities(const SDL_Event& event) noexcept;
}

#endif
