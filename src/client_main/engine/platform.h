#ifndef PLATFORM_H
#define PLATFORM_H

#include "platform/platform_id.h"
#include "platform_types.h"

namespace projectfarm::engine
{
    // returns the current platform
    // if the platform is unknown, a runtime error is thrown
    PlatformTypes GetPlatformType();
}

#endif
