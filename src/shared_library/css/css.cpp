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

    std::string_view SanitizeSelector(std::string_view selector)
    {
        if (selector.size() <= 0)
        {
            return selector;
        }

        if (selector[0] == '.' || selector[0] == '#')
        {
            return selector.substr(1);
        }

        return selector;
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
                auto selector = SanitizeSelector(token.Value);

                currentCSSClass.Selectors.push_back( { type, std::string(selector) } );
            }
        }

        doc.Classes.push_back(currentCSSClass);

        return doc;
    }
}
