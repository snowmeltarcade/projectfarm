#ifndef PROJECTFARM_STATE_H
#define PROJECTFARM_STATE_H

namespace projectfarm::shared::concurrency::state
{
    void SetBool(const std::string& key, bool value) noexcept;

    [[nodiscard]]
    bool GetBool(const std::string& key) noexcept;
}

#endif
