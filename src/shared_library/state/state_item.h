#ifndef PROJECTFARM_STATE_ITEM_H
#define PROJECTFARM_STATE_ITEM_H

#include <string>

namespace projectfarm::shared::state
{
    template <typename T, typename V = std::string>
    class StateItem final
    {
    public:
        StateItem(T key, V value)
        : _key(key),
          _value(value)
        {}
        ~StateItem() = default;

        [[nodiscard]] T GetKey() const noexcept
        {
            return this->_key;
        }

        [[nodiscard]] V GetValue() const noexcept
        {
            return this->_value;
        }

        void SetValue(V value) noexcept
        {
            this->_value = value;
        }

        void Cancel() noexcept
        {
            this->_shouldCancel = true;
        }

        [[nodiscard]] bool GetShouldCancel() const noexcept
        {
            return this->_shouldCancel;
        }

    private:
        T _key;
        V _value;

        bool _shouldCancel {false};
    };

    template <typename T, typename V>
    [[nodiscard]] bool operator== (const StateItem<T, V>& left, const StateItem<T, V>& right) noexcept
    {
        return left.GetKey() == right.GetKey() &&
               left.GetValue() == right.GetValue();
    }

    template <typename T, typename V>
    [[nodiscard]] bool operator!= (const StateItem<T, V>& left, const StateItem<T, V>& right) noexcept
    {
        return !(left == right);
    }
}

#endif
