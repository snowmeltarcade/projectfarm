#include "css_class.h"

namespace projectfarm::shared::css
{
    std::optional<std::string_view> CSSClass::GetAttributeValueByName(std::string_view name) const noexcept
    {
        auto attribute = std::find_if(this->Attributes.begin(), this->Attributes.end(),
                                      [name](const auto& a){ return a.Name == name; });

        if (attribute == this->Attributes.end())
        {
            return {};
        }

        return attribute->Value;
    }

    bool operator==(const CSSClass &lhs, const CSSClass &rhs)
    {
        if (lhs.Selectors.size() != rhs.Selectors.size() ||
            lhs.Attributes.size() != rhs.Attributes.size())
        {
            return false;
        }

        for (auto i {0u}; i < lhs.Selectors.size(); ++i)
        {
            if (lhs.Selectors[i] != rhs.Selectors[i])
            {
                return false;
            }
        }

        for (auto i {0u}; i < lhs.Attributes.size(); ++i)
        {
            if (lhs.Attributes[i] != rhs.Attributes[i])
            {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const CSSClass &lhs, const CSSClass &rhs)
    {
        return !(operator==(lhs, rhs));
    }
}
