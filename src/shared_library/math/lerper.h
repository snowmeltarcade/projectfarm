#ifndef PROJECTFARM_LERPER_H
#define PROJECTFARM_LERPER_H

#include <cstdint>
#include <utility>
#include <cmath>

namespace projectfarm::shared::math
{
    template <typename T>
    class Lerper
    {
    public:
        Lerper() = default;
        ~Lerper() = default;

        void Set(T start, T end, uint64_t milliseconds)
        {
            this->_start = start;
            this->_end = end;

            this->_delta = this->_end - this->_start;

            this->_totalTime = static_cast<float>(milliseconds);
            this->_currentTime = 0;
        }

        std::pair<T, bool> Get(uint64_t frameTime) noexcept
        {
            this->_currentTime += static_cast<float>(frameTime);

            auto value = static_cast<T>(this->_delta * (this->_currentTime / this->_totalTime));

            auto finished = this->_currentTime >= this->_totalTime;

            value = this->_start + value;

            if (this->_end > this->_start)
            {
                if (value < this->_start)
                {
                    value = this->_start;
                }
                else if (value > this->_end)
                {
                    value = this->_end;
                }
            }
            else
            {
                if (value < this->_end)
                {
                    value = this->_end;
                }
                else if (value > this->_start)
                {
                    value = this->_start;
                }
            }

            return {value, finished};
        }

    private:
        T _start;
        T _end;
        T _delta;

        float _totalTime {0.0f};
        float _currentTime {0.0f};
    };
}

#endif
