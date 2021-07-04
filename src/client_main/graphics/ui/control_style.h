#ifndef PROJECTFARM_CONTROL_STYLE_H
#define PROJECTFARM_CONTROL_STYLE_H

#include <optional>
#include <vector>
#include <string>
#include <cstdint>
#include <filesystem>
#include <unordered_map>

#include "logging/consume_logger.h"
#include "css/css.h"
#include "graphics/colors/color.h"
#include "graphics/colors/color_hsv.h"
#include "data/consume_data_provider.h"

using namespace std::literals;

namespace projectfarm::graphics::ui
{
    class ControlStyle final : public shared::ConsumeLogger,
                               public shared::ConsumeDataProvider
    {
    public:
        ControlStyle(shared::css::CSSClass cssClass,
                     std::shared_ptr<shared::Logger> logger,
                     std::shared_ptr<shared::DataProvider> dataProvider) noexcept
        {
            this->SetLogger(logger);
            this->SetDataProvider(dataProvider);

            this->ApplyStyle(std::move(cssClass));
        }
        ~ControlStyle() = default;

        shared::graphics::colors::Color Color {shared::graphics::colors::White};
        shared::graphics::colors::Color TextColor {shared::graphics::colors::White};
        shared::graphics::colors::Color BorderColor {shared::graphics::colors::Transparent};

        std::vector<std::filesystem::path> Textures;

        float Margin {0.0f};
        float Padding {0.0f};

        std::string Font;

    private:
        void ApplyStyle(const shared::css::CSSClass& cssClass) noexcept;

        [[nodiscard]]
        std::optional<shared::graphics::colors::Color>
            GetColorFromStyle(const shared::css::CSSClass& cssClass,
                              const std::string& keyPrefix = "") const noexcept;

        [[nodiscard]]
        std::vector<std::filesystem::path>
            GetTexturesFromStyle(const shared::css::CSSClass& cssClass) const noexcept;

        [[nodiscard]]
        std::optional<float> GetFloatFromStyle(const shared::css::CSSClass& cssClass,
                                               const std::string& name) const noexcept;

        [[nodiscard]]
        std::optional<std::string> GetStringFromStyle(const shared::css::CSSClass& cssClass,
                                                      const std::string& name) const noexcept;

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

        std::optional<uint32_t> GetUInt(std::optional<std::string_view> s) const noexcept;
        std::optional<float> GetFloat(std::optional<std::string_view> s) const noexcept;
        std::optional<std::string> GetString(std::optional<std::string_view> s) const noexcept;
    };
}

#endif
