#ifndef PROJECTFARM_CONTROL_STYLE_H
#define PROJECTFARM_CONTROL_STYLE_H

#include <optional>
#include <vector>
#include <string>

#include "logging/consume_logger.h"
#include "css/css.h"
#include "graphics/colors/color.h"
#include "graphics/colors/color_hsv.h"

using namespace std::literals;

namespace projectfarm::graphics::ui
{
    class ControlStyle final : public shared::ConsumeLogger
    {
    public:
        ControlStyle(shared::css::CSSClass cssClass,
                     std::shared_ptr<shared::Logger> logger) noexcept
        {
            this->SetLogger(logger);
            this->ApplyStyle(std::move(cssClass));
        }
        ~ControlStyle() = default;

    private:
        void ApplyStyle(const shared::css::CSSClass& cssClass) noexcept;

        [[nodiscard]]
        std::optional<shared::graphics::colors::Color>
        GetColorFromStyle(const shared::css::CSSClass& cssClass,
                          const std::string& keyPrefix = "") const noexcept;

        [[nodiscard]]
        std::vector<std::string> GetTexturesFromStyle(const shared::css::CSSClass& cssClass) const noexcept;

        [[nodiscard]]
        std::optional<shared::graphics::colors::Color> GetColorFromStylePropertiesRGBA(
            std::optional<std::string_view> color_rgba,
            std::optional<std::string_view> red,
            std::optional<std::string_view> green,
            std::optional<std::string_view> blue,
            std::optional<std::string_view> alpha) const noexcept;

        [[nodiscard]]
        std::optional<shared::graphics::colors::ColorHSV> GetColorFromStylePropertiesHSV(
            std::optional<std::string_view> color_hsv,
            std::optional<std::string_view> hue,
            std::optional<std::string_view> saturation,
            std::optional<std::string_view> brightness) const noexcept;
    };
}

#endif
