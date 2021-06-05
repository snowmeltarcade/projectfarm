#include "device_capabilities.h"

namespace projectfarm::engine
{
    static DeviceCapabilities deviceCaps;

    DeviceCapabilities GetDeviceCapabilities() noexcept
    {
        return deviceCaps;
    }

    void UpdateDeviceCapabilities(const SDL_Event& event) noexcept
    {
        switch (event.type)
        {
            case SDL_KEYDOWN: [[fallthrough]] ;
            case SDL_KEYUP: [[fallthrough]] ;
            case SDL_TEXTEDITING: [[fallthrough]] ;
            case SDL_TEXTINPUT:
            {
                // this will only work correctly if the touch screen is touched before the keyboard
                // is used, which I think is not easy if the keyboard is on screen
                if (deviceCaps.HasTouchScreen)
                {
                    deviceCaps.HasOnScreenKeyboard = true;
                }
                else
                {
                    deviceCaps.HasPhysicalKeyboard = true;
                }

                break;
            }

            case SDL_MOUSEMOTION: [[fallthrough]] ;
            case SDL_MOUSEBUTTONDOWN: [[fallthrough]] ;
            case SDL_MOUSEBUTTONUP: [[fallthrough]] ;
            case SDL_MOUSEWHEEL:
            {
                deviceCaps.HasPhysicalMouse = true;
                break;
            }

            case SDL_JOYAXISMOTION: [[fallthrough]] ;
            case SDL_JOYBALLMOTION: [[fallthrough]] ;
            case SDL_JOYHATMOTION: [[fallthrough]] ;
            case SDL_JOYBUTTONDOWN: [[fallthrough]] ;
            case SDL_JOYBUTTONUP: [[fallthrough]] ;
            case SDL_JOYDEVICEADDED:
            {
                deviceCaps.HasJoystick = true;
                break;
            }

            case SDL_JOYDEVICEREMOVED:
            {
                // TODO: We should really count the number of joysticks and if 0, then
                // set this to false
                deviceCaps.HasJoystick = false;
                break;
            }

            case SDL_CONTROLLERAXISMOTION: [[fallthrough]] ;
            case SDL_CONTROLLERBUTTONDOWN: [[fallthrough]] ;
            case SDL_CONTROLLERBUTTONUP: [[fallthrough]] ;
            case SDL_CONTROLLERDEVICEADDED: [[fallthrough]] ;
            case SDL_CONTROLLERDEVICEREMAPPED:
            {
                deviceCaps.HasGameController = true;
                break;
            }

            case SDL_CONTROLLERDEVICEREMOVED:
            {
                // TODO: We should really count the number of controllers and if 0, then
                // set this to false
                deviceCaps.HasGameController = false;
                break;
            }

            case SDL_FINGERDOWN: [[fallthrough]] ;
            case SDL_FINGERUP: [[fallthrough]] ;
            case SDL_FINGERMOTION: [[fallthrough]] ;
            case SDL_DOLLARGESTURE: [[fallthrough]] ;
            case SDL_DOLLARRECORD: [[fallthrough]] ;
            case SDL_MULTIGESTURE:
            {
                deviceCaps.HasTouchScreen = true;
                break;
            }

            default:
                break;
        }
    }
}