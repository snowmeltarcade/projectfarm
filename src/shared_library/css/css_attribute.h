#ifndef PROJECTFARM_CSS_ATTRIBUTE_H
#define PROJECTFARM_CSS_ATTRIBUTE_H

#include <string>

namespace projectfarm::shared::css
{
    struct CSSAttribute final
    {
        std::string Name;
        std::string Value;
    };

    bool operator== (const CSSAttribute& lhs, const CSSAttribute& rhs);
    bool operator!= (const CSSAttribute& lhs, const CSSAttribute& rhs);
}

#endif
