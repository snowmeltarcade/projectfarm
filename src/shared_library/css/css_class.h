#ifndef PROJECTFARM_CSS_CLASS_H
#define PROJECTFARM_CSS_CLASS_H

#include <vector>

#include "css_selector.h"
#include "css_attribute.h"

namespace projectfarm::shared::css
{
    struct CSSClass final
    {
        std::vector<CSSSelector> Selectors;
        std::vector<CSSAttribute> Attributes;
    };

    bool operator== (const CSSClass& lhs, const CSSClass& rhs);
    bool operator!= (const CSSClass& lhs, const CSSClass& rhs);
}

#endif
