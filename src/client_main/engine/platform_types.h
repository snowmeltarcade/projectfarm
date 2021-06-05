#ifndef PROJECTFARM_PLATFORM_TYPES_H
#define PROJECTFARM_PLATFORM_TYPES_H

namespace projectfarm::engine
{
    enum class PlatformTypes
    {
        Windows,
        MacOS,
        Linux,
        iOS,
        iOS_Simulator,
        iPadOS,
        iPadOS_Simulator,
        tvOS,
        Android,
        AndroidTablet,
        // will not be used except as a default value
        Unknown,
    };
}

#endif
