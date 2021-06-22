#include <fstream>
#include <sstream>

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
            // TODO: Use expected to return an actual error message
            return {};
        }

        // used for token validation
        auto hasBlockStarted {false};
        auto hasBlockFinished {false};

        CSSDocument doc;

        CSSClass currentCSSClass;
        CSSAttribute currentCSSAttribute;

        for (const auto& token : *tokens)
        {
            hasBlockFinished = false;

            if (token.Type == TokenTypes::Selector)
            {
                auto type = SelectorTypeFromName(token.Value);
                auto selector = SanitizeSelector(token.Value);

                currentCSSClass.Selectors.push_back( { type, std::string(selector) } );
            }
            else if (token.Type == TokenTypes::StartBlock)
            {
                hasBlockStarted = true;
            }
            else if (token.Type == TokenTypes::EndBlock)
            {
                if (!hasBlockStarted)
                {
                    return {};
                }

                hasBlockStarted = false;

                doc.Classes.push_back(currentCSSClass);

                currentCSSClass = CSSClass();

                hasBlockFinished = true;
            }
            else if (token.Type == TokenTypes::AttributeName)
            {
                currentCSSAttribute = CSSAttribute();
                currentCSSAttribute.Name = token.Value;
            }
            else if (token.Type == TokenTypes::AttributeValue)
            {
                currentCSSAttribute.Value = token.Value;

                // ignore invalid attributes
                if (!currentCSSAttribute.Name.empty() &&
                    !currentCSSAttribute.Value.empty())
                {
                    currentCSSClass.Attributes.push_back(currentCSSAttribute);
                }

                currentCSSAttribute = CSSAttribute();
            }
        }

        // looks like a block wasn't finished
        if (!hasBlockFinished)
        {
            return {};
        }

        return doc;
    }

    std::optional<CSSDocument> LoadFromFile(const std::filesystem::path &path) noexcept
    {
        if (!std::filesystem::exists(path))
        {
            return {};
        }

        std::ifstream file(path);

        if (!file.is_open())
        {
            return {};
        }

        std::stringstream ss;
        ss << file.rdbuf();

        auto css = LoadFromRaw(ss.str());
        return css;
    }
}
