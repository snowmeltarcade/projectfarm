#ifndef PROJECTFARM_CHANNEL_H
#define PROJECTFARM_CHANNEL_H

#include <vector>
#include <shared_mutex>

namespace projectfarm::shared::concurrency
{
    // lower case `c` as this class is acting as a namespace to hide `_values`
    template <typename T>
    class channel final
    {
    public:
        // this pushes a copy of `values` into the channel
        void Push(T value) noexcept
        {
            std::unique_lock lock(this->_mutex);

            this->_values.emplace_back(std::move(value));
        }

        [[nodiscard]]
        // this moves all values in the channel to the caller
        std::vector<T> GetAll() noexcept
        {
            std::unique_lock lock(this->_mutex);

            std::vector<T> values;

            for (auto& value : this->_values)
            {
                values.template emplace_back(std::move(value));
            }

            this->_values.clear();

            // this will be a move due to RVO
            return values;
        }

        [[nodiscard]]
        bool HasValues() const noexcept
        {
            std::shared_lock lock(this->_mutex);

            return !this->_values.empty();
        }

    private:
        mutable std::shared_mutex _mutex;
        std::vector<T> _values;
    };
}

#endif
