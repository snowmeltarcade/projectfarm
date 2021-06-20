#ifndef PROJECTFARM_CSS_H
#define PROJECTFARM_CSS_H

#include <string>

#include "css_document.h"

namespace projectfarm::shared::css
{
    [[nodiscard]]
    bool LoadFromRaw(const std::string &path) noexcept;
}

#endif
