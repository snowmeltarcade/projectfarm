#ifndef PROJECTFARM_LABEL_H
#define PROJECTFARM_LABEL_H

#include <string>
#include <SDL.h>

#include "graphics/texture.h"
#include "base_control.h"
#include "graphics/consume_font_manager.h"
#include "graphics/colors/color.h"
#include "math/vector2d.h"
#include "api/logging/logging.h"

namespace projectfarm::graphics::ui
{
    class Label final : public BaseControl,
                        public projectfarm::graphics::ConsumeFontManager
    {
    public:
        Label() = default;
        ~Label() override
        {
            this->Destroy();
        }

        ControlTypes GetControlType() const noexcept override
        {
            return ControlTypes::Label;
        }

        [[nodiscard]]
        bool SetText(std::string_view text,
                     bool forceUpdate = false) noexcept;

        [[nodiscard]]
        const std::string& GetText() const noexcept
        {
            return this->_text;
        }

        void Clear() override;

        void Render() override;

        void Destroy();

        [[nodiscard]]
        bool SetupFromJson(const nlohmann::json& controlJson,
                           const std::shared_ptr<UI>& ui,
                           const std::vector<std::pair<std::string, std::string>>& parameters) override;

        [[nodiscard]]
        std::string Script_GetText() const noexcept override
        {
            return this->_text;
        }

        void Script_SetText(std::string_view text) noexcept override
        {
            if (!this->SetText(text))
            {
                shared::api::logging::Log("Failed to set text: " + std::string(text));
            }
        }

        [[nodiscard]]
        uint32_t Script_GetCustomPropertyInt(std::string_view name,
                                             const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept override;

        [[nodiscard]]
        bool Script_GetCustomPropertyBool(std::string_view name,
                                          const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept override;

        [[nodiscard]]
        shared::math::Vector2D Script_GetCustomPropertyVector2D(std::string_view name,
                                                                std::string_view parameter) noexcept override;

    protected:
        void ApplyStyle(bool isLoading) noexcept override;

    private:
        std::shared_ptr<graphics::Texture> _backgroundTexture;

        std::string _text;
        Font::RenderDetails _renderDetails;

        std::string _characterOverride;

        bool _useMarkdown {true};
        bool _allowNewLines {true};
        bool _scrollToBottomOnTextAdd {false};

        uint32_t _maxLines {0};

        [[nodiscard]]
        uint32_t Script_GetCustomPropertyInt_font_line_height() noexcept;

        [[nodiscard]]
        shared::math::Vector2D Script_GetCustomProperty_character_pos(std::string_view parameter) noexcept;

        [[nodiscard]]
        uint32_t Script_GetCustomPropertyInt_rendered_text_length() noexcept;

        [[nodiscard]]
        uint32_t Script_GetCustomPropertyInt_current_character_position_from_x_y_pos(
                const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept;

        void OnDrag(uint32_t, uint32_t, uint32_t dx, uint32_t dy) noexcept override;
    };
}

#endif
