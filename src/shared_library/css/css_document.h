#ifndef PROJECTFARM_CSS_DOCUMENT_H
#define PROJECTFARM_CSS_DOCUMENT_H

#include <vector>

#include "css_class.h"

namespace projectfarm::shared::css
{
    struct CSSDocument final
    {
        std::vector<CSSClass> Classes;
    };

    bool operator== (const CSSDocument& lhs, const CSSDocument& rhs);
    bool operator!= (const CSSDocument& lhs, const CSSDocument& rhs);
}

#endif
