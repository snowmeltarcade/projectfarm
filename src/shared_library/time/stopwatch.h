#ifndef PROJECTFARM_STOPWATCH_H
#define PROJECTFARM_STOPWATCH_H

#include <cstdint>
#include <chrono>
#include <functional>

namespace projectfarm::shared::time
{
    class Stopwatch final
    {
    public:
        Stopwatch() = default;
        ~Stopwatch() = default;

        void Start() noexcept;
        void Stop() noexcept;
        void Reset() noexcept;

        void SetOnTick(const std::function<void(void)>& onTick) noexcept
        {
            this->_onTick = onTick;
        }

        bool Tick() noexcept;

        void SetTargetMilliseconds(uint64_t milliseconds) noexcept
        {
            this->_targetMicroseconds = milliseconds * 1000u;
        }

        [[nodiscard]] uint64_t GetTargetMicroseconds() const noexcept
        {
            return this->_targetMicroseconds;
        }

        [[nodiscard]] uint64_t GetTargetMilliseconds() const noexcept
        {
            return this->_targetMicroseconds / 1000u;
        }

    private:
        uint64_t _targetMicroseconds {0u};
        uint64_t _currentMicroseconds {0u};
        std::chrono::steady_clock::time_point _lastTime;

        std::function<void(void)> _onTick;

        bool _started {false};
    };
}

#endif
