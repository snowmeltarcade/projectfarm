#include "css_attribute.h"

namespace projectfarm::shared::css
{
    bool operator==(const CSSAttribute &lhs, const CSSAttribute &rhs)
    {
        return lhs.Name == rhs.Name &&
               lhs.Value == rhs.Value;
    }

    bool operator!=(const CSSAttribute &lhs, const CSSAttribute &rhs)
    {
        return !(operator==(lhs, rhs));
    }
}
