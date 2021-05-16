#include "logger.h"
#include "time/clock.h"

namespace projectfarm::shared
{
    void Logger::LogMessage(std::string_view s) const noexcept
    {
        std::scoped_lock<std::mutex> lock(this->_logMessageMutex);

        auto currentTime = time::Clock::LocalTimeAsLongString();

        std::cout << currentTime << ": " << s << std::endl;
    }
}