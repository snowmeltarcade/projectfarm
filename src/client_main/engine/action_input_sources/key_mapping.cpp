#include "key_mapping.h"

namespace projectfarm::engine::action_input_sources
{
    [[nodiscard]] bool operator== (const KeyMapping::Binding& left, const KeyMapping::Binding& right) noexcept
    {
        return left._actionType == right._actionType && left._keys == right._keys;
    }

    [[nodiscard]] bool operator!= (const KeyMapping::Binding& left, const KeyMapping::Binding& right) noexcept
    {
        return !(left == right);
    }
}