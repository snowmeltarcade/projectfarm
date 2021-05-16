#ifndef PROJECTFARM_TEXT_PARTS_H
#define PROJECTFARM_TEXT_PARTS_H

#include <string>

#include "graphics/colors/color.h"

namespace projectfarm::shared::markdown
{
    enum class TextPartTypes
    {
        Normal,
        Italic,
        Bold,
        BoldItalic,
    };

    struct TextPart
    {
        TextPartTypes _type {TextPartTypes::Normal};
        // the starting position of the non-control text as a text position
        uint32_t _originalTextStartIndex {0u};
        // the position one place after the last control character in the full text
        uint32_t _endTextPosition {0u};
        std::string _text;
        graphics::colors::Color _color {graphics::colors::Black};
    };

    constexpr bool operator==(const TextPart& lhs, const TextPart& rhs) noexcept
    {
        return lhs._type == rhs._type &&
               lhs._originalTextStartIndex == rhs._originalTextStartIndex &&
               lhs._endTextPosition == rhs._endTextPosition &&
               lhs._text == rhs._text &&
               lhs._color == rhs._color;
    }

    constexpr bool operator!=(const TextPart& lhs, const TextPart& rhs) noexcept
    {
        return !(lhs == rhs);
    }
}

#endif
