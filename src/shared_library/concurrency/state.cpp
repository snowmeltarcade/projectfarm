#include <unordered_map>
#include <string>
#include <shared_mutex>

#include "state.h"

namespace projectfarm::shared::concurrency::state
{
    std::shared_mutex _mutex;

    std::unordered_map<std::string, bool> _boolMap;

    void SetBool(const std::string& key, bool value) noexcept
    {
        std::unique_lock lock(_mutex);

        _boolMap[key] = value;
    }

    bool GetBool(const std::string& key) noexcept
    {
        std::shared_lock lock(_mutex);

        return _boolMap[key];
    }
}
