#include "timer.h"

namespace projectfarm::shared::time
{
    void Timer::Reset()
    {
        this->_totalFrames = 0;
        this->_totalGameDuration = 0;
        this->_lastFrameDuration = 0;
        this->_fps = 0;
        this->_fpsCounter = 0;
        this->_fpsDurationCounter = 0;

        this->_baseTime = std::chrono::steady_clock::now();
        this->_lastFrameTime = this->_baseTime;
    }

    void Timer::IncrementFrame()
    {
        auto currentTime = std::chrono::steady_clock::now();

        this->_totalFrames++;

        auto gameTime = currentTime - this->_baseTime;
        this->_totalGameDuration =
            static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(gameTime).count());

        auto lastFrameTime = currentTime - this->_lastFrameTime;
        this->_lastFrameTime = currentTime;
        this->_lastFrameDuration =
            static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(lastFrameTime).count());

        this->_fpsDurationCounter += this->_lastFrameDuration;
        if (this->_fpsDurationCounter >= 1000000)
        {
            this->_fps = this->_fpsCounter;
            this->_fpsCounter = 0;
            this->_fpsDurationCounter = 0;
        }
        this->_fpsCounter++;
    }
}