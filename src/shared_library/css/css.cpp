#include "css.h"
#include "tokenizer.h"

namespace projectfarm::shared::css
{
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
                currentCSSClass.Selectors.push_back( { CSSSelectorTypes::Type, token.Value } );
            }
        }

        doc.Classes.push_back(currentCSSClass);

        return doc;
    }
}
