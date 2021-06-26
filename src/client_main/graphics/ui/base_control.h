#ifndef PROJECTFARM_BASE_CONTROL_H
#define PROJECTFARM_BASE_CONTROL_H

#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <string>
#include <nlohmann/json.hpp>

#include "logging/consume_logger.h"
#include "graphics/consume_graphics.h"
#include "data/consume_data_provider.h"
#include "graphics/ui/consume_ui.h"
#include "control_size.h"
#include "control_position.h"
#include "engine/scripting/ui_control_script.h"
#include "math/vector2d.h"
#include "graphics/texture.h"
#include "fit_types.h"
#include "control_types.h"
#include "css/css.h"
#include "graphics/colors/color_hsv.h"

namespace projectfarm::graphics::ui
{
    class UI;

    class BaseControl : public projectfarm::graphics::ConsumeGraphics,
                        public projectfarm::shared::ConsumeLogger,
                        public projectfarm::shared::ConsumeDataProvider,
                        public ConsumeUI,
                        public std::enable_shared_from_this<BaseControl>
    {
    public:
        using EventHandlerType = std::function<void(const std::shared_ptr<const BaseControl>& control,
                                                    uint32_t x,
                                                    uint32_t y)>;

        BaseControl() = default;
        ~BaseControl() override = default;

        virtual ControlTypes GetControlType() const noexcept = 0;

        virtual void Clear() = 0;

        void ClearChildren() noexcept;

        virtual void Render() = 0;

        void RenderChildren() const noexcept;

        [[nodiscard]]
        virtual bool SetupFromJson(const nlohmann::json& controlJson,
                                   const std::shared_ptr<UI>& ui,
                                   const std::vector<std::pair<std::string, std::string>>& parameters) = 0;

        void AddChild(const std::shared_ptr<BaseControl>& child) noexcept
        {
            this->_children.emplace_back(child);
            child->SetParent(this->shared_from_this());
        }

        [[nodiscard]]
        const ControlSize& GetSize() const noexcept
        {
            return this->_size;
        }

        void SetSize(const ControlSize& size) noexcept;

        [[nodiscard]]
        const ControlPosition& GetPosition() const noexcept
        {
            return this->_position;
        }

        void SetPosition(const ControlPosition& position) noexcept;

        void SetParent(const std::shared_ptr<BaseControl>& parent) noexcept
        {
            this->_parent = parent;
        }

        [[nodiscard]]
        const std::shared_ptr<BaseControl>& GetParent() const noexcept
        {
            return this->_parent;
        }

        void SetIsVisible(bool isVisible) noexcept
        {
            this->_isVisible = isVisible;
        }

        [[nodiscard]]
        bool GetIsVisible() const noexcept
        {
            return this->_isVisible;
        }

        void SetIsHidden(bool isHidden) noexcept
        {
            this->_isHidden = isHidden;
        }

        [[nodiscard]]
        bool GetIsHidden() const noexcept
        {
            return this->_isHidden;
        }

        void SetIsEnabled(bool isEnabled) noexcept
        {
            this->_isEnabled = isEnabled;
        }

        [[nodiscard]]
        bool GetIsEnabled() const noexcept;

        void HandleClick(uint32_t x, uint32_t y) noexcept;
        virtual void OnClick(uint32_t, uint32_t) noexcept
        {
        }

        void HandleMouseEnter(uint32_t x, uint32_t y) noexcept;
        virtual void OnMouseEnter(uint32_t, uint32_t) noexcept
        {
        }

        void HandleMouseLeave(uint32_t x, uint32_t y) noexcept;
        virtual void OnMouseLeave(uint32_t, uint32_t) noexcept
        {
        }

        void HandleDrag(uint32_t x, uint32_t y, uint32_t dx, uint32_t dy) noexcept;
        virtual void OnDrag(uint32_t, uint32_t, uint32_t, uint32_t) noexcept
        {
        }

        void HandleKeyPress() noexcept;
        virtual void OnKeyPress() noexcept
        {
        }

        void HandleKeyPress_Enter() noexcept;
        virtual void OnKeyPress_Enter() noexcept
        {
        }

        [[nodiscard]]
        bool IsPointInControl(uint32_t x, uint32_t y) const noexcept;

        [[nodiscard]]
        bool IsVisibleInParentControl() const noexcept;

        void SetOnClickEventHandler(const EventHandlerType& handler) noexcept
        {
            this->_onClickEvent = handler;
        }

        [[nodiscard]]
        const std::shared_ptr<shared::scripting::Script>& GetInlineScript() const noexcept
        {
            return this->_inlineScript;
        }

        [[nodiscard]]
        const std::shared_ptr<shared::scripting::Script>& GetExternalScript() const noexcept
        {
            return this->_externalScript;
        }

        void SetInlineScript(const std::shared_ptr<engine::scripting::UIControlScript>& script) noexcept
        {
            this->_inlineScript = script;
        }

        void SetExternalScript(const std::shared_ptr<engine::scripting::UIControlScript>& script) noexcept
        {
            this->_externalScript = script;
        }

        void CallScriptFunction(shared::scripting::FunctionTypes functionType,
                                const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters,
                                bool callChildren) noexcept;

        [[nodiscard]]
        bool CallScriptFunction(const std::string& functionName,
                                const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept;

        [[nodiscard]]
        std::optional<std::shared_ptr<BaseControl>> GetFocusedControlAtPoint(uint32_t x, uint32_t y) noexcept;

        [[nodiscard]]
        virtual std::string Script_GetText() const noexcept
        {
            return "";
        }

        virtual void Script_SetText(std::string_view) noexcept
        {
        }

        [[nodiscard]]
        virtual std::string Script_GetCustomPropertyString(std::string_view,
                                                           const std::vector<projectfarm::shared::scripting::FunctionParameter>&) noexcept
        {
            return "";
        }

        [[nodiscard]]
        virtual uint32_t Script_GetCustomPropertyInt(std::string_view,
                                                     const std::vector<projectfarm::shared::scripting::FunctionParameter>&) noexcept
        {
            return 0;
        }

        [[nodiscard]]
        virtual bool Script_GetCustomPropertyBool(std::string_view,
                                                  const std::vector<projectfarm::shared::scripting::FunctionParameter>&) noexcept
        {
            return false;
        }

        [[nodiscard]]
        virtual shared::math::Vector2D Script_GetCustomPropertyVector2D(std::string_view, std::string_view) noexcept
        {
            return {};
        }

        [[nodiscard]]
        std::optional<std::shared_ptr<BaseControl>> FindControlById(const std::string& id) noexcept;

        [[nodiscard]]
        const std::string& GetId() const noexcept
        {
            return this->_id;
        }

        void SetId(std::string_view id) noexcept
        {
            this->_id = id;
        }

        [[nodiscard]]
        SDL_Rect GetScreenSpaceBounds() const noexcept;

        [[nodiscard]]
        bool DoesSupportJavascript() const noexcept
        {
            return this->_doesSupportJavascript;
        }

        [[nodiscard]]
        bool RefreshStyles() noexcept;

        void ApplyStyle(const shared::css::CSSClass& cssClass) noexcept;

    private:
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

        // no [[nodiscard]]
        bool CallScriptFunction(const std::shared_ptr<shared::scripting::Script>& script,
                                const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters,
                                shared::scripting::FunctionTypes functionType) noexcept;

        // no [[nodiscard]]
        bool CallScriptFunction(const std::shared_ptr<shared::scripting::Script>& script,
                                const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters,
                                const std::string& functionName) noexcept;

        void SetParentSize(const ControlSize& size) noexcept;

    protected:
        [[nodiscard]]
        bool SetCommonValuesFromJson(const nlohmann::json& json) noexcept;

        [[nodiscard]]
        bool CreateMask(uint32_t width = 0, uint32_t height = 0) noexcept;

        [[nodiscard]]
        bool UpdateMaskToParentBoundary(uint32_t width = 0, uint32_t height = 0) noexcept;

        [[nodiscard]]
        SDL_Rect GetMaskBoundary() const noexcept;

        void UpdateVisibility() noexcept;

        std::vector<std::shared_ptr<BaseControl>> _children;

        ControlSize _size;
        ControlPosition _position;
        FitTypes _fitType { FitTypes::Fit };

        std::string _id;

        // Has this control explicitly been set to visible or not by
        // a user or caller. A control may be not be visible even if the
        // parent control is visible and this control is in the renderable
        // portion of the screen
        bool _isVisible {true};

        // Has this control been hidden by the system. The parent control
        // may not be visible, but this control may still be set to visible.
        // If so, this control should not be rendered.
        bool _isHidden {false};

        // Should this control respond to interactions and UI events or not.
        bool _isEnabled {true};

        // can this control be focus?
        // the mouse cursor cannot be for instance
        bool _canFocus {true};

        // does this control load javascript events?
        // custom controls do not for instance, as the
        // Javascript events are forwarded to the internal
        // controls
        bool _doesSupportJavascript {true};

        std::string _cssClass;

        std::shared_ptr<graphics::Texture> _maskTexture;
        std::shared_ptr<SDLFreeableSurface> _maskSurface;

        std::shared_ptr<BaseControl> _parent;

        EventHandlerType _onClickEvent;

        std::shared_ptr<shared::scripting::Script> _inlineScript;
        std::shared_ptr<shared::scripting::Script> _externalScript;
    };
}

#endif
