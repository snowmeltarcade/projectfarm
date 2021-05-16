#ifndef PROJECTFARM_MARKDOWN_H
#define PROJECTFARM_MARKDOWN_H

#include <vector>

#include "text_parts.h"

namespace projectfarm::shared::markdown
{
    [[nodiscard]]
    std::vector<TextPart> GetTextParts(std::string_view text,
                                       graphics::colors::Color defaultColor = graphics::colors::Black) noexcept;

    // this converts a position in the text part text to a position in the full markdown text
    [[nodiscard]]
    uint32_t GetMarkdownPositionFromPartPosition(std::string_view text, uint32_t partPosition) noexcept;
}

#endif