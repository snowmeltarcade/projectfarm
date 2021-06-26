#include "control_style.h"

namespace projectfarm::graphics::ui
{
    void ControlStyle::ApplyStyle(const shared::css::CSSClass& cssClass) noexcept
    {
        auto color = this->GetColorFromStyle(cssClass);
        auto borderColor = this->GetColorFromStyle(cssClass, "border-");

        auto textures = this->GetTexturesFromStyle(cssClass);

        // get margin and padding. they are floats...
        // perhaps put in a `layout` struct or something?
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

        try
        {
            if (red)
            {
                auto value = std::stoi(std::string(*red));
                if (value < 0 || value > 255)
                {
                    this->LogMessage("Red value is invalid: " + std::to_string(value));
                    return {};
                }

                color.r = value;
            }

            if (green)
            {
                auto value = std::stoi(std::string(*green));
                if (value < 0 || value > 255)
                {
                    this->LogMessage("Green value is invalid: " + std::to_string(value));
                    return {};
                }

                color.g = value;
            }

            if (blue)
            {
                auto value = std::stoi(std::string(*blue));
                if (value < 0 || value > 255)
                {
                    this->LogMessage("Blue value is invalid: " + std::to_string(value));
                    return {};
                }

                color.b = value;
            }

            if (alpha)
            {
                auto value = std::stoi(std::string(*alpha));
                if (value < 0 || value > 255)
                {
                    this->LogMessage("Alpha value is invalid: " + std::to_string(value));
                    return {};
                }

                color.a = value;
            }
        }
        catch (const std::exception& ex)
        {
            this->LogMessage("Failed to get color with error: "s + ex.what());
            return {};
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

        try
        {
            if (hue)
            {
                auto value = std::stof(std::string(*hue));
                if (value < 0.0f || value > 1.0f)
                {
                    this->LogMessage("Hue value is invalid: " + std::to_string(value));
                    return {};
                }

                color.h = value;
            }

            if (saturation)
            {
                auto value = std::stof(std::string(*saturation));
                if (value < 0.0f || value > 1.0f)
                {
                    this->LogMessage("Saturation value is invalid: " + std::to_string(value));
                    return {};
                }

                color.s = value;
            }

            if (brightness)
            {
                auto value = std::stof(std::string(*brightness));
                if (value < 0.0f || value > 1.0f)
                {
                    this->LogMessage("Brightness value is invalid: " + std::to_string(value));
                    return {};
                }

                color.v = value;
            }
        }
        catch (const std::exception& ex)
        {
            this->LogMessage("Failed to get color with error: "s + ex.what());
            return {};
        }

        return color;
    }
}
