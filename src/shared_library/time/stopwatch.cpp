#include "stopwatch.h"

namespace projectfarm::shared::time
{
    void Stopwatch::Start() noexcept
    {
        this->_started = true;
        this->_lastTime = std::chrono::steady_clock::now();
    }

    void Stopwatch::Stop() noexcept
    {
        this->_started = false;
    }

    void Stopwatch::Reset() noexcept
    {
        this->Stop();

        this->_currentMicroseconds = 0u;
        this->_onTick = {};
    }

    bool Stopwatch::Tick() noexcept
    {
        if (!this->_started)
        {
            return false;
        }

        auto currentTime = std::chrono::steady_clock::now();
        auto duration = currentTime - this->_lastTime;
        this->_lastTime = currentTime;

        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

        if (microseconds == 0)
        {
            return false;
        }

        this->_currentMicroseconds += microseconds;

        if (this->_currentMicroseconds >= this->_targetMicroseconds)
        {
            if (this->_onTick)
            {
                this->_onTick();
            }

            this->_currentMicroseconds = 0u;

            return true;
        }

        return false;
    }
}