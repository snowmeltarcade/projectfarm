#ifndef PROJECTFARM_CONSUME_TIMER_H
#define PROJECTFARM_CONSUME_TIMER_H

#include <memory>

namespace projectfarm::shared::time
{
    class Timer;

    class ConsumeTimer
    {
    public:
        ConsumeTimer() = default;
        virtual ~ConsumeTimer() = default;

        [[nodiscard]] const std::shared_ptr<Timer>& GetTimer() const
        {
            return _timer;
        }

        void SetTimer(const std::shared_ptr<Timer>& timer)
        {
            this->_timer = timer;
        }

    private:
        std::shared_ptr<Timer> _timer;
    };
}

#endif
