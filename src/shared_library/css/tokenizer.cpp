#include <cctype>

#include "tokenizer.h"

namespace projectfarm::shared::css
{
    static const auto BlockOpenCharacter = '{';
    static const auto BlockCloseCharacter = '}';
    static const auto SelectorSeparatorCharacter = ',';

    enum class ParseMode
    {
        Selector,
        Block,
    };

    static auto CurrentParseMode = ParseMode::Selector;

    bool GetNextCharacter(std::string_view css, uint32_t& pos, char& c)
    {
        while (pos < css.size())
        {
            c = css[pos];

            if (std::isspace(c))
            {
                ++pos;
                continue;
            }

            return true;
        }

        // we're at the end of the text, so nothing more to do
        return false;
    }

    std::optional<std::vector<Token>> ParseTokens(std::string_view css) noexcept
    {
        // 3 characters is the minimum valid length - a selector and an empty block:
        // s{}
        if (css.size() <= 2)
        {
            return {};
        }

        std::vector<Token> tokens;

        std::string value;

        auto saveSelector = [&value, &tokens](auto pos)
        {
            tokens.push_back( { TokenTypes::Selector, pos - static_cast<uint32_t>(value.size()), value } );
            value = "";
        };

        for (auto pos {0u}; pos < css.size(); ++pos)
        {
            auto c {'\0'};
            if (!GetNextCharacter(css, pos, c))
            {
                break;
            }

            if (CurrentParseMode == ParseMode::Selector)
            {
                if (c == SelectorSeparatorCharacter)
                {
                    saveSelector(pos);

                    continue;
                }
                else if (c == BlockOpenCharacter)
                {
                    saveSelector(pos);

                    CurrentParseMode = ParseMode::Block;
                    continue;
                }
            }
            else if (CurrentParseMode == ParseMode::Block)
            {
                if (c == BlockCloseCharacter)
                {
                    CurrentParseMode = ParseMode::Selector;

                    value = "";
                    continue;
                }
            }

            value += c;
        }

        return tokens;
    }
}