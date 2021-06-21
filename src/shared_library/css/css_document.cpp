#include "css_document.h"

namespace projectfarm::shared::css
{
    bool operator== (const CSSDocument& lhs, const CSSDocument& rhs)
    {
        if (lhs.Classes.size() != rhs.Classes.size())
        {
            return false;
        }

        for (auto i {0u}; i < lhs.Classes.size(); ++i)
        {
            if (lhs.Classes[i] != rhs.Classes[i])
            {
                return false;
            }
        }

        return true;
    }

    bool operator!= (const CSSDocument& lhs, const CSSDocument& rhs)
    {
        return !(operator==(lhs, rhs));
    }
}
