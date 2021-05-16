#ifndef PROJECTFARM_RANDOM_ENGINE_H
#define PROJECTFARM_RANDOM_ENGINE_H

#include <cstdint>
#include <chrono>
#include <random>

namespace projectfarm::shared::math
{
    class RandomEngine
    {
    public:
        RandomEngine() = default;
        ~RandomEngine() = default;

        void Initialize(uint64_t seed = 0u) noexcept
        {
            this->_engine.seed(seed == 0u ? static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
                                          : static_cast<unsigned int>(seed));
        }

        [[nodiscard]] uint32_t Next(uint32_t min, uint32_t max) noexcept
        {
            std::uniform_int_distribution<uint32_t> dist(min, max);
            return dist(this->_engine);
        }

        [[nodiscard]] int32_t Next(int32_t min, int32_t max) noexcept
        {
            std::uniform_int_distribution<int32_t> dist(min, max);
            return dist(this->_engine);
        }

        [[nodiscard]] uint64_t Next(uint64_t min, uint64_t max) noexcept
        {
            std::uniform_int_distribution<uint64_t> dist(min, max);
            return dist(this->_engine);
        }

        [[nodiscard]] int64_t Next(int64_t min, int64_t max) noexcept
        {
            std::uniform_int_distribution<int64_t> dist(min, max);
            return dist(this->_engine);
        }

        [[nodiscard]] float Next(float min, float max) noexcept
        {
            std::uniform_real_distribution<float> dist(min, max);
            return dist(this->_engine);
        }

        [[nodiscard]] double Next(double min, double max) noexcept
        {
            std::uniform_real_distribution<double> dist(min, max);
            return dist(this->_engine);
        }

    private:
        std::default_random_engine _engine;
    };
}

#endif
