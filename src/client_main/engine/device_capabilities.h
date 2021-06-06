#ifndef PROJECTFARM_DEVICE_CAPABILITIES_H
#define PROJECTFARM_DEVICE_CAPABILITIES_H

#include <SDL.h>

namespace projectfarm::engine
{
    struct DeviceCapabilities
    {
        bool HasTouchScreen = false;
        bool HasPhysicalKeyboard = false;
        bool HasOnScreenKeyboard = false;
        bool HasPhysicalMouse = false;
        bool HasGameController = false;
        bool HasJoystick = false;
    };

    [[nodiscard]]
    DeviceCapabilities GetDeviceCapabilities() noexcept;

    void UpdateDeviceCapabilities(const SDL_Event& event) noexcept;
}

#endif
