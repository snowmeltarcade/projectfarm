#ifndef PROJECTFARM_KEY_MAPPING_H
#define PROJECTFARM_KEY_MAPPING_H

#include <cstdint>
#include <vector>
#include <algorithm>

#include "engine/action_types.h"

namespace projectfarm::engine::action_input_sources
{
    class KeyMapping final
    {
    public:
        KeyMapping() = default;
        ~KeyMapping() = default;

        void AddBinding(uint32_t key, engine::ActionTypes action) noexcept
        {
            this->AddBinding({key}, action);
        }

        void AddBinding(std::initializer_list<uint32_t> keys, engine::ActionTypes action) noexcept
        {
            // sort the bindings, with most keys first
            Binding binding { keys, action };
            this->_bindings.emplace_back(std::move(binding));
        }

        struct Binding
        {
            std::vector<uint32_t> _keys;
            engine::ActionTypes _actionType;
        };

        [[nodiscard]] const std::vector<Binding>& GetBindings() const noexcept
        {
            return this->_bindings;
        }

    private:
        std::vector<Binding> _bindings;
    };

    [[nodiscard]] bool operator== (const KeyMapping::Binding& left, const KeyMapping::Binding& right) noexcept;
    [[nodiscard]] bool operator!= (const KeyMapping::Binding& left, const KeyMapping::Binding& right) noexcept;
}

#endif
