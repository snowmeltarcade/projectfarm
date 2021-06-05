#include <stdexcept>
#include <string>
#include <SDL.h>

#include "platform.h"

using namespace std::literals;

namespace projectfarm::engine
{
    PlatformTypes GetPlatformType()
    {
        auto platformName = SDL_GetPlatform();
        auto isTablet = SDL_IsTablet();

        if (platformName == "Android"s)
        {
            return isTablet ? PlatformTypes::AndroidTablet : PlatformTypes::Android;
        }
        else if (platformName == "Linux"s)
        {
            return PlatformTypes::Linux;
        }
        else if (platformName == "Mac OS X"s)
        {
            return PlatformTypes::MacOS;
        }
        else if (platformName == "Windows"s)
        {
            return PlatformTypes::Windows;
        }
        else if (platformName == "tvOS"s)
        {
            return PlatformTypes::tvOS;
        }
        else if (platformName == "iOS"s)
        {
#if TARGET_OS_SIMULATOR == 1
            return isTablet ? PlatformTypes::iPadOS_Simulator : PlatformTypes::iOS_Simulator;
#else
            return isTablet ? PlatformTypes::iPadOS : PlatformTypes::iOS;
#endif
        }

        throw std::runtime_error("Unsupported platform: `"s + platformName + "`. Is tablet: "s + (isTablet ? "True" : "False"));
    }
}
