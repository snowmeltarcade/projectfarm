#ifndef PROJECTFARM_CUSTOM_H
#define PROJECTFARM_CUSTOM_H

#include <string>

#include "graphics/texture.h"
#include "base_control.h"
#include "graphics/consume_font_manager.h"
#include "graphics/colors/color.h"

namespace projectfarm::graphics::ui
{
    class Custom final : public BaseControl,
                         public projectfarm::graphics::ConsumeFontManager
    {
    public:
        Custom()
        {
            // custom controls do not handle javascript events, but
            // will forward javascript events to internal controls.
            // this needs to be set in the constructor as this value
            // is checked before this control is actually loaded by
            // the UI framework
            this->_doesSupportJavascript = false;
        }
        ~Custom() override = default;

        ControlTypes GetControlType() const noexcept override
        {
            return ControlTypes::Custom;
        }

        const std::string& GetName() const noexcept
        {
            return this->_name;
        }

        void Clear() override;

        void Render() override;

        void ReadIdFromJson(const nlohmann::json& controlJson,
                            const std::shared_ptr<UI>& ui,
                            const std::vector<std::pair<std::string, std::string>>& parameters) override;

        [[nodiscard]]
        bool SetupFromJson(const nlohmann::json& controlJson,
                           const std::shared_ptr<UI>& ui,
                           const std::vector<std::pair<std::string, std::string>>& parameters) override;

        [[nodiscard]]
        std::string Script_GetText() const noexcept override
        {
            return this->_textControl ? this->_textControl->Script_GetText() : "";
        }

        void Script_SetText(std::string_view text) noexcept override
        {
            if (this->_textControl)
            {
                this->_textControl->Script_SetText(text);
            }
        }

        [[nodiscard]]
        std::string Script_GetCustomPropertyString(std::string_view key,
                                                   const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept override;

    private:
        [[nodiscard]]
        auto GetControlParameters(const nlohmann::json& parentControlJson,
                                  const nlohmann::json& controlJson,
                                  const std::vector<std::pair<std::string, std::string>>& parentParameters)
            -> std::optional<std::vector<std::pair<std::string, std::string>>>;

        void ProcessMeta(const nlohmann::json& json) noexcept;

        [[nodiscard]]
        std::string TransformParameterValueFromStyle(const std::string& value) const noexcept;

        // the control that handles `GetText` and `SetText` type commands
        std::shared_ptr<BaseControl> _textControl;

        std::string _type;

        // the name (or the `type`) of this custom control
        std::string _name;
    };
}

#endif
