#include "control_style.h"

namespace projectfarm::graphics::ui
{
    void ControlStyle::ApplyStyle(const shared::css::CSSClass& cssClass) noexcept
    {
        if (auto c = this->GetColorFromStyle(cssClass); c)
        {
            this->Color = *c;
        }

        if (auto c = this->GetColorFromStyle(cssClass, "border-"); c)
        {
            this->BorderColor = *c;
        }

        this->Textures = this->GetTexturesFromStyle(cssClass);

        if (auto v = this->GetFloatFromStyle(cssClass, "padding"); v)
        {
            this->Padding = *v;
        }

        if (auto v = this->GetFloatFromStyle(cssClass, "margin"); v)
        {
            this->Margin = *v;
        }
    }

    std::optional<shared::graphics::colors::Color> ControlStyle::GetColorFromStyle(
        const shared::css::CSSClass& cssClass,
        const std::string& keyPrefix) const noexcept
    {
        auto color = cssClass.GetAttributeValueByName(keyPrefix + "color");
        auto color_rgba = cssClass.GetAttributeValueByName(keyPrefix + "color-rgba");
        auto color_hsv = cssClass.GetAttributeValueByName(keyPrefix + "color-hsv");
        auto hue = cssClass.GetAttributeValueByName(keyPrefix + "hue");
        auto saturation = cssClass.GetAttributeValueByName(keyPrefix + "saturation");
        auto brightness = cssClass.GetAttributeValueByName(keyPrefix + "brightness");
        auto red = cssClass.GetAttributeValueByName(keyPrefix + "red");
        auto green = cssClass.GetAttributeValueByName(keyPrefix + "green");
        auto blue = cssClass.GetAttributeValueByName(keyPrefix + "blue");
        auto alpha = cssClass.GetAttributeValueByName(keyPrefix + "alpha");

        // `color` is an alias for `color-rgb`
        if (!color_rgba)
        {
            color_rgba = color;
        }

        shared::graphics::colors::Color rgba = shared::graphics::colors::White;

        if (auto c = this->GetColorFromStylePropertiesRGBA(color_rgba, red, green, blue, alpha); c)
        {
            rgba = *c;
        }
        else
        {
            this->LogMessage("Failed to get color RGBA style.");
            return {};
        }

        // `color_rgba` overrides `color_hsv`
        // any components of hsv (hue, saturation, brightness) that were defines
        // will still be used
        if (color_rgba)
        {
            color_hsv = {};
        }

        shared::graphics::colors::ColorHSV hsv;

        if (auto c = this->GetColorFromStylePropertiesHSV(color_hsv, hue, saturation, brightness); c)
        {
            hsv = *c;
        }
        else
        {
            this->LogMessage("Failed to get color HSV style.");
            return {};
        }

        // apply hsv components if used
        auto resultingColor = shared::graphics::colors::Mix(rgba, hsv);

        return resultingColor;
    }

    std::vector<std::string> ControlStyle::GetTexturesFromStyle(const shared::css::CSSClass& cssClass) const noexcept
    {
        std::vector<std::string> textures;

        constexpr auto maxTextures = 16u;

        // `texture` is an alias for `texture0`
        auto firstTextureName = ""s;
        if (auto name = cssClass.GetAttributeValueByName("texture"); name)
        {
            firstTextureName = *name;
        }

        for (auto i {0u}; i < maxTextures; ++i)
        {
            auto name = "texture" + std::to_string(i);

            if (auto n = cssClass.GetAttributeValueByName(name); n)
            {
                if (i == 0)
                {
                    // we'll add this later on
                    firstTextureName = *n;
                    continue;
                }

                textures.push_back(std::string(*n));
            }
        }

        // add in the alias if it exists
        if (!firstTextureName.empty())
        {
            textures.insert(textures.begin(), firstTextureName);
        }

        return textures;
    }

    std::optional<float> ControlStyle::GetFloatFromStyle(const shared::css::CSSClass& cssClass,
                                                         const std::string& name) const noexcept
    {
        auto attributeValue = cssClass.GetAttributeValueByName(name);
        if (!attributeValue)
        {
            return {};
        }

        if (auto value = this->GetFloat(attributeValue); value)
        {
            return *value;
        }

        return {};
    }

    std::optional<shared::graphics::colors::Color> ControlStyle::GetColorFromStylePropertiesRGBA(
        std::optional<std::string_view> color_rgba,
        std::optional<std::string_view> red,
        std::optional<std::string_view> green,
        std::optional<std::string_view> blue,
        std::optional<std::string_view> alpha) const noexcept
    {
        shared::graphics::colors::Color color = shared::graphics::colors::White;

        if (color_rgba)
        {
            if (auto c = shared::graphics::colors::FromString(*color_rgba); c)
            {
                color = *c;
            }
            else
            {
                this->LogMessage("Invalid color: "s + std::string(*color_rgba));
                return {};
            }
        }

        if (auto value = this->GetUInt(red); value)
        {
            if (*value < 0 || *value > 255)
            {
                this->LogMessage("Red value is invalid: " + std::to_string(*value));
                return {};
            }

            color.r = *value;
        }

        if (auto value = this->GetUInt(green); value)
        {
            if (*value < 0 || *value > 255)
            {
                this->LogMessage("Green value is invalid: " + std::to_string(*value));
                return {};
            }

            color.g = *value;
        }

        if (auto value = this->GetUInt(blue); value)
        {
            if (*value < 0 || *value > 255)
            {
                this->LogMessage("Blue value is invalid: " + std::to_string(*value));
                return {};
            }

            color.b = *value;
        }

        if (auto value = this->GetUInt(alpha); value)
        {
            if (*value < 0 || *value > 255)
            {
                this->LogMessage("Alpha value is invalid: " + std::to_string(*value));
                return {};
            }

            color.a = *value;
        }

        return color;
    }

    std::optional<shared::graphics::colors::ColorHSV> ControlStyle::GetColorFromStylePropertiesHSV(
        std::optional<std::string_view> color_hsv,
        std::optional<std::string_view> hue,
        std::optional<std::string_view> saturation,
        std::optional<std::string_view> brightness) const noexcept
    {
        shared::graphics::colors::ColorHSV color;

        if (color_hsv)
        {
            if (auto c = shared::graphics::colors::FromStringHSV(*color_hsv); c)
            {
                color = *c;
            }
            else
            {
                this->LogMessage("Invalid color: "s + std::string(*color_hsv));
                return {};
            }
        }

        if (auto value = this->GetFloat(hue); value)
        {
            if (*value < 0.0f || *value > 1.0f)
            {
                this->LogMessage("Hue value is invalid: " + std::to_string(*value));
                return {};
            }

            color.h = *value;
        }

        if (auto value = this->GetFloat(saturation); value)
        {
            if (*value < 0.0f || *value > 1.0f)
            {
                this->LogMessage("Saturation value is invalid: " + std::to_string(*value));
                return {};
            }

            color.s = *value;
        }

        if (auto value = this->GetFloat(brightness); value)
        {
            if (*value < 0.0f || *value > 1.0f)
            {
                this->LogMessage("Brightness value is invalid: " + std::to_string(*value));
                return {};
            }

            color.v = *value;
        }

        return color;
    }

    std::optional<uint32_t> ControlStyle::GetUInt(std::optional<std::string_view> s) const noexcept
    {
        if (!s)
        {
            return {};
        }

        try
        {
            auto value = std::stoi(std::string(*s));
            return value;
        }
        catch (const std::exception& ex)
        {
            return {};
        }
    }

    std::optional<float> ControlStyle::GetFloat(std::optional<std::string_view> s) const noexcept
    {
        if (!s)
        {
            return {};
        }

        try
        {
            auto value = std::stof(std::string(*s));
            return value;
        }
        catch (const std::exception& ex)
        {
            return {};
        }
    }
}
