#include "css_selector.h"

namespace projectfarm::shared::css
{
    bool operator== (const CSSSelector& lhs, const CSSSelector& rhs)
    {
        return lhs.Type == rhs.Type &&
               lhs.Name == rhs.Name;
    }

    bool operator!= (const CSSSelector& lhs, const CSSSelector& rhs)
    {
        return !(operator==(lhs, rhs));
    }
}
