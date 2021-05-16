#include <ctime>
#include <iomanip>
#include <sstream>

#include "clock.h"

namespace projectfarm::shared::time
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> Clock::DurationSinceEpoch() noexcept
    {
        auto now = std::chrono::system_clock::now();

        auto milliseconds = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

        return milliseconds;
    }

    uint64_t Clock::MillisecondsSinceEpoch() noexcept
    {
        auto milliseconds = Clock::DurationSinceEpoch();

        auto millisecondsSinceEpoch = milliseconds.time_since_epoch().count();

        return millisecondsSinceEpoch;
    }

    std::string Clock::UTCAsShortString() noexcept
    {
        auto s = Clock::UTCAsString(Clock::ShortTimeFormatString);
        return s;
    }

    std::string Clock::UTCAsLongString() noexcept
    {
        auto s = Clock::UTCAsString(Clock::LongTimeFormatString);
        return s;
    }

    // See https://en.cppreference.com/w/cpp/io/manip/put_time for format string
    std::string Clock::UTCAsString(const std::string& format) noexcept
    {
        auto milliseconds = Clock::DurationSinceEpoch();

        auto now = milliseconds.time_since_epoch().count();

        auto s = Clock::UTCAsString(format, now);
        return s;
    }

    std::string Clock::UTCAsString(const std::string& format, uint64_t milliseconds) noexcept
    {
        std::chrono::milliseconds millisecondsDuration(milliseconds);
        std::chrono::time_point<std::chrono::system_clock> duration(millisecondsDuration);

        auto now = std::chrono::system_clock::to_time_t(duration);

        tm tm;
#ifdef WIN32
        gmtime_s(&tm, &now);
#else
        gmtime_r(&now, &tm);
#endif

        std::stringstream ss;
        ss << std::put_time(&tm, format.c_str());

        return ss.str();
    }

    std::string Clock::LocalTimeAsShortString() noexcept
    {
        auto s = Clock::LocalTimeAsString(Clock::ShortTimeFormatString);
        return s;
    }

    std::string Clock::LocalTimeAsLongString() noexcept
    {
        auto s = Clock::LocalTimeAsString(Clock::LongTimeFormatString);
        return s;
    }

    std::string Clock::LocalTimeAsShortString(uint64_t milliseconds) noexcept
    {
        auto s = Clock::LocalTimeAsString(Clock::ShortTimeFormatString, milliseconds);
        return s;
    }

    std::string Clock::LocalTimeAsLongString(uint64_t milliseconds) noexcept
    {
        auto s = Clock::LocalTimeAsString(Clock::LongTimeFormatString, milliseconds);
        return s;
    }

    // See https://en.cppreference.com/w/cpp/io/manip/put_time for format string
    std::string Clock::LocalTimeAsString(const std::string& format) noexcept
    {
        auto milliseconds = Clock::DurationSinceEpoch();

        auto now = milliseconds.time_since_epoch().count();

        auto s = Clock::LocalTimeAsString(format, now);
        return s;
    }

    std::string Clock::LocalTimeAsString(const std::string& format, uint64_t milliseconds) noexcept
    {
        std::chrono::milliseconds millisecondsDuration(milliseconds);
        std::chrono::time_point<std::chrono::system_clock> duration(millisecondsDuration);

        auto now = std::chrono::system_clock::to_time_t(duration);

        tm tm;
#ifdef WIN32
        localtime_s(&tm, &now);
#else
        localtime_r(&now, &tm);
#endif

        std::stringstream ss;
        ss << std::put_time(&tm, format.c_str());

        return ss.str();
    }
}
