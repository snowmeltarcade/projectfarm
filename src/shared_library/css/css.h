#ifndef PROJECTFARM_CSS_H
#define PROJECTFARM_CSS_H

#include <string>
#include <filesystem>
#include <optional>

#include "css_document.h"

namespace projectfarm::shared::css
{
    [[nodiscard]]
    std::optional<CSSDocument> LoadFromRaw(const std::string &css) noexcept;

    [[nodiscard]]
    std::optional<CSSDocument> LoadFromFile(const std::filesystem::path &path) noexcept;
}

#endif
