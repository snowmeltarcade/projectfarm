#ifndef PROJECTFARM_CSS_SELECTOR_H
#define PROJECTFARM_CSS_SELECTOR_H

#include <string>

#include "css_selector_types.h"

namespace projectfarm::shared::css
{
    struct CSSSelector final
    {
        CSSSelectorTypes Type { CSSSelectorTypes::Class };
        std::string Name;
    };

    bool operator== (const CSSSelector& lhs, const CSSSelector& rhs);
    bool operator!= (const CSSSelector& lhs, const CSSSelector& rhs);
}

#endif
