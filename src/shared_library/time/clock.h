#ifndef PROJECTFARM_CLOCK_H
#define PROJECTFARM_CLOCK_H

#include <cstdint>
#include <chrono>
#include <string>

namespace projectfarm::shared::time
{
    class Clock final
    {
    public:
        static std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> DurationSinceEpoch() noexcept;

        static uint64_t MillisecondsSinceEpoch() noexcept;

        static std::string UTCAsShortString() noexcept;
        static std::string UTCAsLongString() noexcept;
        static std::string UTCAsString(const std::string& format) noexcept;
        static std::string UTCAsString(const std::string& format, uint64_t milliseconds) noexcept;

        static std::string LocalTimeAsShortString() noexcept;
        static std::string LocalTimeAsLongString() noexcept;
        static std::string LocalTimeAsShortString(uint64_t milliseconds) noexcept;
        static std::string LocalTimeAsLongString(uint64_t milliseconds) noexcept;
        static std::string LocalTimeAsString(const std::string& format) noexcept;
        static std::string LocalTimeAsString(const std::string& format, uint64_t milliseconds) noexcept;

    private:
        // 2021-04-25 17:51
        static inline constexpr auto ShortTimeFormatString = "%y-%m-%d %H:%M";

        // Sunday 25 April, 2021, 17:51
        static inline constexpr auto LongTimeFormatString = "%A %d %B, %Y, %H:%M";
    };
}

#endif
