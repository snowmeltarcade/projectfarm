#ifndef PROJECTFARM_ITEM_H
#define PROJECTFARM_ITEM_H

#include <string>
#include <unordered_map>
#include <vector>

namespace projectfarm::shared::api::config
{
    struct Item
    {
        std::string Key;
        std::string Value;

        std::vector<Item> Children;
    };

    struct ItemRoot
    {
        std::vector<Item> Children;
    };

    inline bool operator== (const Item& lhs, const Item& rhs)
    {
        return lhs.Key == rhs.Key &&
               lhs.Value == rhs.Value &&
               lhs.Children == rhs.Children;
    }

    inline bool operator!= (const Item& lhs, const Item& rhs)
    {
        return !(operator== (lhs, rhs));
    }

    inline bool operator== (const ItemRoot& lhs, const ItemRoot& rhs)
    {
        return lhs.Children == rhs.Children;
    }

    inline bool operator!= (const ItemRoot& lhs, const ItemRoot& rhs)
    {
        return !(operator== (lhs, rhs));
    }
}

#endif
