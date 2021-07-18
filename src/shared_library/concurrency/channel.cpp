#include "channel.h"

namespace projectfarm::shared::concurrency
{
    template <typename T>
    std::mutex channel<T>::_mutex;

    template <typename T>
    std::unordered_map<std::string, std::vector<T>> channel<T>::_values;
}