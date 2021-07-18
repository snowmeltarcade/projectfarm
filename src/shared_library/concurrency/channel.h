#ifndef PROJECTFARM_CHANNEL_H
#define PROJECTFARM_CHANNEL_H

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace projectfarm::shared::concurrency
{
    // lower case `c` as this class is acting as a namespace to hide `_values`
    template <typename T>
    class channel final
    {
    public:
        // this pushes a copy of `values` into the channel
        static void Push(const std::string& key, T value) noexcept
        {
            std::scoped_lock lock(channel::_mutex);

            channel::_values[key].emplace_back(std::move(value));
        }

        [[nodiscard]]
        // this moves all values in the channel to the caller
        static std::vector<T> GetAll(const std::string& key) noexcept
        {
            std::scoped_lock lock(channel::_mutex);

            std::vector<T> values;

            for (auto& value : channel::_values[key])
            {
                values.template emplace_back(std::move(value));
            }

            channel::_values[key].clear();

            return values;
        }

    private:
        static std::mutex _mutex;
        static std::unordered_map<std::string, std::vector<T>> _values;
    };
}

#endif
