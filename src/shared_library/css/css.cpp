#include "css.h"
#include "tokenizer.h"

namespace projectfarm::shared::css
{
    CSSSelectorTypes SelectorTypeFromName(std::string_view name)
    {
        if (name.size() <= 0)
        {
            return CSSSelectorTypes::Type;
        }

        if (name[0] == '.')
        {
            return CSSSelectorTypes::Class;
        }
        else if (name[0] == '#')
        {
            return CSSSelectorTypes::Id;
        }

        return CSSSelectorTypes::Type;
    }

    std::optional<CSSDocument> LoadFromRaw(const std::string &css) noexcept
    {
        auto tokens = ParseTokens(css);
        if (!tokens)
        {
            return {};
        }

        CSSDocument doc;

        CSSClass currentCSSClass;

        for (const auto& token : *tokens)
        {
            if (token.Type == TokenTypes::Selector)
            {
                auto type = SelectorTypeFromName(token.Value);

                currentCSSClass.Selectors.push_back( { type, token.Value } );
            }
        }

        doc.Classes.push_back(currentCSSClass);

        return doc;
    }
}
