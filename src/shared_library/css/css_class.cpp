#include "css_class.h"

namespace projectfarm::shared::css
{
    bool operator==(const CSSClass &lhs, const CSSClass &rhs)
    {
        if (lhs.Selectors.size() != rhs.Selectors.size())
        {
            return false;
        }

        for (auto i {0u}; i < lhs.Selectors.size(); ++i)
        {
            if (lhs.Selectors[i] != rhs.Selectors[i])
            {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const CSSClass &lhs, const CSSClass &rhs)
    {
        return !(operator==(lhs, rhs));
    }
}