#include <iostream>
#include <mutex>

#include "logging.h"
#include "time/clock.h"

namespace projectfarm::shared::api::logging
{
    void Log(std::string_view message, LogLevels) noexcept
    {
        static std::mutex mutex;

        std::scoped_lock<std::mutex> lock(mutex);

        auto currentTime = time::Clock::LocalTimeAsLongString();

        std::cout << currentTime << ": " << message << '\n';
    }
}
