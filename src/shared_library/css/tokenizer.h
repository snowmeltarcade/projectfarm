#ifndef PROJECTFARM_TOKENIZER_H
#define PROJECTFARM_TOKENIZER_H

#include <cstdint>
#include <optional>
#include <vector>
#include <string>

namespace projectfarm::shared::css
{
    enum class TokenTypes : uint8_t
    {
        Selector,
    };

    struct Token
    {
        TokenTypes Type {TokenTypes::Selector};
        uint32_t Pos {0};
        std::string Value;
    };

    std::optional<std::vector<Token>> ParseTokens(std::string_view css) noexcept;
}

#endif
