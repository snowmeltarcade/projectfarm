#ifndef PROJECTFARM_TIMER_H
#define PROJECTFARM_TIMER_H

#include <cstdlib>
#include <chrono>

namespace projectfarm::shared::time
{
    class Timer final
    {
    public:
        Timer()
        {
            this->Reset();
        }
        ~Timer() = default;

        void Reset();

        void IncrementFrame();

        [[nodiscard]]
        uint64_t GetTotalGameDurationInMicroseconds() const
        {
            return this->_totalGameDuration;
        }

        [[nodiscard]]
        uint64_t GetLastFrameDurationInMicroseconds() const
        {
            return this->_lastFrameDuration;
        }

        [[nodiscard]]
        uint64_t GetLastFrameDurationInMilliseconds() const
        {
            return this->_lastFrameDuration / 1000u;
        }

        [[nodiscard]]
        float GetLastFrameDurationInSeconds() const
        {
            return 0.000001f * this->GetLastFrameDurationInMicroseconds();
        }

        [[nodiscard]] uint64_t GetFPS() const
        {
            return this->_fps;
        }

    private:
        uint64_t _totalFrames {0};

        std::chrono::steady_clock::time_point _baseTime;

        uint64_t _totalGameDuration {0};

        std::chrono::steady_clock::time_point _lastFrameTime;
        uint64_t _lastFrameDuration {0};

        uint64_t _fps {0};
        uint64_t _fpsCounter {0};
        uint64_t _fpsDurationCounter {0};
    };
}

#endif
