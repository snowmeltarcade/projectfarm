#include <algorithm>

#include "base_control.h"
#include "graphics/graphics.h"
#include "scripting/function_types.h"
#include "custom.h"
#include "api/logging/logging.h"

using namespace std::literals;

namespace projectfarm::graphics::ui
{
    void BaseControl::ClearChildren() noexcept
    {
        for (const auto& child : this->_children)
        {
            child->Clear();
        }

        this->_children.clear();
    }

    void BaseControl::RenderChildren() const noexcept
    {
        for (const auto& child : this->_children)
        {
            if (!child->GetIsVisible() ||
                child->GetIsHidden())
            {
                continue;
            }

            child->Render();
        }
    }

    void BaseControl::SetSize(const ControlSize& size) noexcept
    {
        this->_size = size;

        // recreate the mask as the surface size now needs to be different
        if (this->_maskSurface)
        {
            this->_maskSurface->FreeSurface();
            this->_maskSurface = {};
        }

        for (auto& child : this->_children)
        {
            child->SetParentSize(this->_size);
        }

        this->UpdateVisibility();
    }

    void BaseControl::SetParentSize(const ControlSize& size) noexcept
    {
        this->_size.SetParentSize(size);
        this->_size.ReconfirmSize();

        for (auto& child : this->_children)
        {
            child->SetParentSize(this->_size);
        }
    }

    void BaseControl::SetPosition(const ControlPosition& position) noexcept
    {
        this->_position = position;

        this->UpdateVisibility();
    }

    bool BaseControl::GetIsEnabled() const noexcept
    {
        // this control itself may be enabled, but if a parent control
        // is not enabled, then this control does not count as enabled
        if (this->_parent && !this->_parent->GetIsEnabled())
        {
            return false;
        }

        return this->_isEnabled;
    }

    void BaseControl::HandleClick(uint32_t x, uint32_t y) noexcept
    {
        if (this->IsPointInControl(x, y))
        {
            auto cx {0u};
            auto cy {0u};
            this->_position.ResolvePosition(this->shared_from_this(), cx, cy);
            x -= cx;
            y -= cy;

            if (this->_onClickEvent)
            {
                this->_onClickEvent(this->shared_from_this(), x, y);
            }

            this->CallScriptFunction(shared::scripting::FunctionTypes::OnClick,
                                     {
                                        { projectfarm::shared::scripting::FunctionParameterTypes::Int, std::to_string(x) },
                                        { projectfarm::shared::scripting::FunctionParameterTypes::Int, std::to_string(y) },
                                     },
                                     false);

            this->OnClick(x, y);
        }
    }

    void BaseControl::HandleMouseEnter(uint32_t x, uint32_t y) noexcept
    {
        if (this->IsPointInControl(x, y))
        {
            auto cx {0u};
            auto cy {0u};
            this->_position.ResolvePosition(this->shared_from_this(), cx, cy);
            x -= cx;
            y -= cy;

            this->CallScriptFunction(shared::scripting::FunctionTypes::OnMouseEnter,
                                     {
                                         { projectfarm::shared::scripting::FunctionParameterTypes::Int, std::to_string(x) },
                                         { projectfarm::shared::scripting::FunctionParameterTypes::Int, std::to_string(y) },
                                     },
                                     false);

            this->OnMouseEnter(x, y);
        }
    }

    void BaseControl::HandleMouseLeave(uint32_t x, uint32_t y) noexcept
    {
        auto cx {0u};
        auto cy {0u};
        this->_position.ResolvePosition(this->shared_from_this(), cx, cy);
        x -= cx;
        y -= cy;

        // don't check if the point is not in this control, as the point may
        // over a child control
        this->CallScriptFunction(shared::scripting::FunctionTypes::OnMouseLeave,
                                 {
                                     { projectfarm::shared::scripting::FunctionParameterTypes::Int, std::to_string(x) },
                                     { projectfarm::shared::scripting::FunctionParameterTypes::Int, std::to_string(y) },
                                 },
                                 false);

        this->OnMouseLeave(x, y);
    }

    void BaseControl::HandleDrag(uint32_t x, uint32_t y, uint32_t dx, uint32_t dy) noexcept
    {
        // no scrolling for controls unless they are free in size
        if (this->_fitType != FitTypes::Free)
        {
            return;
        }

        // we don't check if the point is in the control, so if the control is dragged
        // and then the cursor moves outside of the control's boundary, the drag event
        // can continue

        auto cx {0u};
        auto cy {0u};
        this->_position.ResolvePosition(this->shared_from_this(), cx, cy);
        x -= cx;
        y -= cy;

        this->CallScriptFunction(shared::scripting::FunctionTypes::OnDrag,
                                 {
                                     { projectfarm::shared::scripting::FunctionParameterTypes::Int, std::to_string(x) },
                                     { projectfarm::shared::scripting::FunctionParameterTypes::Int, std::to_string(y) },
                                 },
                                 false);

        this->OnDrag(x, y, dx, dy);
    }

    void BaseControl::HandleKeyPress() noexcept
    {
        this->CallScriptFunction(shared::scripting::FunctionTypes::OnKeyPress,
                                 {},
                                 false);

        this->OnKeyPress();
    }

    void BaseControl::HandleKeyPress_Enter() noexcept
    {
        this->CallScriptFunction(shared::scripting::FunctionTypes::OnKeyPress_Enter,
                                 {},
                                 false);

        this->OnKeyPress_Enter();
    }

    bool BaseControl::IsPointInControl(uint32_t x, uint32_t y) const noexcept
    {
        auto xPos {0u};
        auto yPos {0u};

        this->_position.ResolvePosition(this->shared_from_this(), xPos, yPos);

        auto result = x >= xPos && x <= xPos + this->_size.GetWidth() &&
                      y >= yPos && y <= yPos + this->_size.GetHeight();

        return result;
    }

    bool BaseControl::IsVisibleInParentControl() const noexcept
    {
        if (!this->_parent)
        {
            return true;
        }

        auto local = this->GetScreenSpaceBounds();
        auto parentMask = this->_parent->GetMaskBoundary();

        auto isInParentControl = SDL_HasIntersection(&local, &parentMask);
        return isInParentControl;
    }

    void BaseControl::CallScriptFunction(shared::scripting::FunctionTypes functionType,
                                         const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters,
                                         bool callChildren) noexcept
    {
        // inline scripts take priority over external scripts
        if (!this->CallScriptFunction(this->_inlineScript, parameters, functionType))
        {
            this->CallScriptFunction(this->_externalScript, parameters, functionType);
        }

        if (callChildren)
        {
            for (const auto& c : this->_children)
            {
                c->CallScriptFunction(functionType, parameters, callChildren);
            }
        }
    }

    SDL_Rect BaseControl::GetScreenSpaceBounds() const noexcept
    {
        auto x {0u};
        auto y {0u};

        this->_position.ResolvePosition(this->shared_from_this(), x, y);

        SDL_Rect rect
        {
            static_cast<int>(x),
            static_cast<int>(y),
            static_cast<int>(this->_size.GetWidth()),
            static_cast<int>(this->_size.GetHeight()),
        };

        return rect;
    }

    bool BaseControl::RefreshStyles(bool isLoading, const std::optional<ControlStyle>& parentStyle) noexcept
    {
        std::optional<shared::css::CSSClass> cssClass;

        auto type = this->GetControlType();

        // the control type of a custom control is just `custom`. We
        // want the actual type of control
        auto typeName = type == ControlTypes::Custom ?
            static_cast<const Custom*>(this)->GetName() :
            ControlTypesToString(type);

        // check the control type, then id then css class in this order
        // and allow the latter to override the former
        if (auto css = this->_ui->GetStyles()->GetBySelectorAndType(typeName,
                                                                    shared::css::CSSSelectorTypes::Type);
            css)
        {
            cssClass = css;
        }

        if (auto css = this->_ui->GetStyles()->GetBySelectorAndType(this->_id,
                                                                    shared::css::CSSSelectorTypes::Id);
            css)
        {
            cssClass = css;
        }

        if (!this->_cssClass.empty())
        {
            if (auto css = this->_ui->GetStyles()->GetBySelectorAndType(this->_cssClass,
                                                                        shared::css::CSSSelectorTypes::Class);
                css)
            {
                cssClass = css;
            }
            else
            {
                shared::api::logging::Log("Failed to find css class with selector: " + this->_cssClass);
                return false;
            }
        }

        if (isLoading && parentStyle)
        {
            // take on the parent style if loading, as this child control
            // likely wants to ignore type styles from default.css etc...
            this->_style = std::make_shared<ControlStyle>(*parentStyle);
        }
        else
        {
            // override any previous styles
            if (cssClass)
            {
                this->_style = std::make_shared<ControlStyle>(*cssClass, this->_dataProvider);
            }
        }

        this->ApplyStyle(isLoading);

        return true;
    }

    bool BaseControl::CallScriptFunction(const std::shared_ptr<shared::scripting::Script>& script,
                                         const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters,
                                         shared::scripting::FunctionTypes functionType) noexcept
    {
        if (script &&
            script->DoesFunctionExist(functionType) &&
            script->CallFunction(functionType, parameters)) // TODO: Handle error when function is called
        {
            // this may not exist, so just ignore any errors
            return true;
        }

        return false;
    }

    bool BaseControl::CallScriptFunction(const std::shared_ptr<shared::scripting::Script>& script,
                                         const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters,
                                         const std::string& functionName) noexcept
    {
        if (script &&
            script->DoesFunctionExist(functionName) &&
            script->CallFunction(functionName, parameters)) // TODO: Handle error when function is called
        {
            // this may not exist, so just ignore any errors
            return true;
        }

        return false;
    }

    bool BaseControl::CallScriptFunction(const std::string& functionName,
                                         const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept
    {
        // inline scripts take priority over external scripts
        if (!this->CallScriptFunction(this->_inlineScript, parameters, functionName))
        {
            if (!this->CallScriptFunction(this->_externalScript, parameters, functionName))
            {
                shared::api::logging::Log("Failed to find function with name: " + functionName);
                return false;
            }
        }

        return true;
    }

    std::optional<std::shared_ptr<BaseControl>> BaseControl::GetFocusedControlAtPoint(uint32_t x, uint32_t y) noexcept
    {
        if (!this->_canFocus ||
            !this->GetIsVisible() ||
            this->GetIsHidden() ||
            !this->GetIsEnabled() ||
            !this->IsPointInControl(x, y))
        {
            return {};
        }

        // we want to check from the last control in the list, as that
        // will be rendered on top
        for (auto child = this->_children.rbegin(); child != this->_children.rend(); ++child)
        {
            auto c = (*child)->GetFocusedControlAtPoint(x, y);
            if (c)
            {
                return c;
            }
        }

        return this->shared_from_this();
    }

    bool BaseControl::SetCommonValuesFromJson(const nlohmann::json& json) noexcept
    {
        if (auto id = json.find("id"); id != json.end())
        {
            this->_id = id->get<std::string>();
        }

        if (auto isVisible = json.find("isVisible"); isVisible != json.end())
        {
            // use this as a string to allow for parameters in uesr controls
            this->_isVisible = isVisible->get<std::string>() == "true";
        }

        if (auto canFocus = json.find("canFocus"); canFocus != json.end())
        {
            // use this as a string to allow for parameters in uesr controls
            this->_canFocus = canFocus->get<std::string>() == "true";
        }

        if (auto fitType = json.find("fitType"); fitType != json.end())
        {
            auto name = fitType->get<std::string>();
            if (auto type = StringToFitTypes(name); !type)
            {
                shared::api::logging::Log("Invalid fit type value: " + name);
                return false;
            }
            else
            {
                this->_fitType = *type;
            }
        }

        if (auto renderOriginPoint = json.find("renderOriginPoint"); renderOriginPoint != json.end())
        {
            auto name = renderOriginPoint->get<std::string>();
            if (auto type = StringToRenderOriginPoints(name); !type)
            {
                shared::api::logging::Log("Invalid fit type value: " + name);
                return false;
            }
            else
            {
                this->_position.SetRenderOriginPoint(*type);
            }
        }

        return true;
    }

    std::optional<std::shared_ptr<BaseControl>> BaseControl::FindControlById(const std::string& id) noexcept
    {
        if (this->_id == id)
        {
            return this->shared_from_this();
        }

        for (const auto& child : this->_children)
        {
            auto c = child->FindControlById(id);
            if (c)
            {
                return c;
            }
        }

        return {};
    }

    bool BaseControl::CreateMask(uint32_t width, uint32_t height) noexcept
    {
        if (!this->_maskTexture)
        {
            this->_maskTexture = std::make_shared<graphics::Texture>();
            this->_maskTexture->SetGraphics(this->GetGraphics());
        }

        return this->UpdateMaskToParentBoundary(width, height);
    }

    bool BaseControl::UpdateMaskToParentBoundary(uint32_t width, uint32_t height) noexcept
    {
        if (!this->_maskTexture)
        {
            return true;
        }

        if (width == 0)
        {
            width = this->GetSize().GetWidth();
        }

        if (height == 0)
        {
            height = this->GetSize().GetHeight();
        }

        shared::graphics::colors::Color transparentWhite { 255, 255, 255, 0 };

        if (this->_maskSurface)
        {
            this->GetGraphics()->StampSurface(this->_maskSurface->GetSurface(), transparentWhite);
        }
        else
        {
            this->_maskSurface = this->GetGraphics()->CreateSurface(transparentWhite,
                                                                    width, height);
        }

        auto localRect = this->GetScreenSpaceBounds();

        auto [offsetX, offsetY] = this->_position.GetOffset();

        auto mask = this->GetMaskBoundary();

        shapes::Rectangle stampRectangle
        {
            mask.x - localRect.x - offsetX,
            mask.y - localRect.y - offsetY,
            static_cast<uint32_t>(mask.w),
            static_cast<uint32_t>(mask.h),
            shared::graphics::colors::White,
        };

        // stamp the new position
        this->GetGraphics()->StampSurface(this->_maskSurface->GetSurface(), stampRectangle);

        if (!this->_maskTexture->LoadFromSurface(this->_maskSurface))
        {
            shared::api::logging::Log("Failed to create mask texture.");
            return false;
        }

        return true;
    }

    SDL_Rect BaseControl::GetMaskBoundary() const noexcept
    {
        SDL_Rect mask;

        auto localRect = this->GetScreenSpaceBounds();

        if (this->_parent)
        {
            auto parentMask = this->_parent->GetMaskBoundary();

            if (!SDL_IntersectRect(&parentMask, &localRect, &mask))
            {
                // no intersection with parent, so just return the local rect
                return localRect;
            }
        }
        else
        {
            mask = localRect;
        }

        return mask;
    }

    void BaseControl::UpdateVisibility() noexcept
    {
        if (this->IsVisibleInParentControl())
        {
            this->SetIsHidden(false);

            if (!this->UpdateMaskToParentBoundary())
            {
                shared::api::logging::Log("Failed to create mask from parent boundary.");
                return;
            }
        }
        else
        {
            this->SetIsHidden(true);
        }

        for (const auto& child : this->_children)
        {
            child->UpdateVisibility();
        }
    }
}