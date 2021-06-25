#ifndef PROJECTFARM_TEXTURE_H
#define PROJECTFARM_TEXTURE_H

#include <memory>
#include <filesystem>

#include "graphics/ui/canvas.h"
#include "graphics/texture.h"
#include "graphics/colors/color.h"
#include "base_control.h"

namespace projectfarm::graphics::ui
{
    class Texture final : public BaseControl
    {
    public:
        Texture() = default;
        ~Texture() override = default;

        [[nodiscard]]
        bool Load(const std::filesystem::path& texturePath) noexcept;

        ControlTypes GetControlType() const noexcept override
        {
            return ControlTypes::Texture;
        }

        void Clear() override;

        void Render() override;

        [[nodiscard]]
        bool SetupFromJson(const nlohmann::json& controlJson,
                           const std::shared_ptr<UI>& ui,
                           const std::vector<std::pair<std::string, std::string>>& parameters) override;

        void ApplyStyle(const shared::css::CSSClass& cssClass) noexcept override;

        void SetColor(const shared::graphics::colors::Color& color) noexcept
        {
            if (this->_backgroundTexture)
            {
                this->_backgroundTexture->SetColor(color);
            }
        }

    private:
        void OnDrag(uint32_t, uint32_t, uint32_t dx, uint32_t dy) noexcept override;

        std::shared_ptr<graphics::Texture> _backgroundTexture;
    };
}

#endif
