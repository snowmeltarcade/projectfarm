#include "ui_base_control_script_object.h"
#include "scripting/math/vector2d_script_object.h"
#include "scripting/script_system.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::scripting
{
    v8::Persistent<v8::ObjectTemplate> UIBaseControlScriptObject::_controlTemplate;

    v8::Local<v8::Object> UIBaseControlScriptObject::GetObjectTemplateInstance(v8::Isolate* isolate,
                                                                               graphics::ui::BaseControl* control) noexcept
    {
        auto controlTemplate = UIBaseControlScriptObject::GetObjectTemplate(isolate);

        auto context = isolate->GetCurrentContext();

        auto instance = controlTemplate->NewInstance(context).ToLocalChecked();
        instance->SetInternalField(UIBaseControlScriptObject::InternalFieldIndex_Control,
                                   v8::External::New(isolate, control));

        return instance;
    }

    v8::Local<v8::ObjectTemplate> UIBaseControlScriptObject::GetObjectTemplate(v8::Isolate* isolate) noexcept
    {
        if (!UIBaseControlScriptObject::_controlTemplate.IsEmpty())
        {
            return UIBaseControlScriptObject::_controlTemplate.Get(isolate);
        }

        auto controlTemplate = v8::ObjectTemplate::New(isolate);

        controlTemplate->SetInternalFieldCount(1);

        controlTemplate->SetAccessor(shared::scripting::Script::StringToParameter("id", isolate),
                                     UIBaseControlScriptObject::IdGetter,
                                     UIBaseControlScriptObject::IdSetter);

        controlTemplate->SetAccessor(shared::scripting::Script::StringToParameter("text", isolate),
                                     UIBaseControlScriptObject::TextGetter,
                                     UIBaseControlScriptObject::TextSetter);

        controlTemplate->SetAccessor(shared::scripting::Script::StringToParameter("width", isolate),
                                     UIBaseControlScriptObject::WidthGetter,
                                     UIBaseControlScriptObject::WidthSetter);

        controlTemplate->SetAccessor(shared::scripting::Script::StringToParameter("height", isolate),
                                     UIBaseControlScriptObject::HeightGetter,
                                     UIBaseControlScriptObject::HeightSetter);

        controlTemplate->SetAccessor(shared::scripting::Script::StringToParameter("pos_x", isolate),
                                     UIBaseControlScriptObject::PosXGetter,
                                     UIBaseControlScriptObject::PosXSetter);

        controlTemplate->SetAccessor(shared::scripting::Script::StringToParameter("pos_y", isolate),
                                     UIBaseControlScriptObject::PosYGetter,
                                     UIBaseControlScriptObject::PosYSetter);

        controlTemplate->SetAccessor(shared::scripting::Script::StringToParameter("is_visible", isolate),
                                     UIBaseControlScriptObject::IsVisibleGetter,
                                     UIBaseControlScriptObject::IsVisibleSetter);

        controlTemplate->SetAccessor(shared::scripting::Script::StringToParameter("is_enabled", isolate),
                                     UIBaseControlScriptObject::IsEnabledGetter,
                                     UIBaseControlScriptObject::IsEnabledSetter);

        controlTemplate->Set(v8::String::NewFromUtf8(isolate, "get_custom_property_string").ToLocalChecked(),
                             v8::FunctionTemplate::New(isolate, &UIBaseControlScriptObject::GetCustomPropertyString));

        controlTemplate->Set(v8::String::NewFromUtf8(isolate, "get_custom_property_int").ToLocalChecked(),
                             v8::FunctionTemplate::New(isolate, &UIBaseControlScriptObject::GetCustomPropertyInt));

        controlTemplate->Set(v8::String::NewFromUtf8(isolate, "get_custom_property_bool").ToLocalChecked(),
                             v8::FunctionTemplate::New(isolate, &UIBaseControlScriptObject::GetCustomPropertyBool));

        controlTemplate->Set(v8::String::NewFromUtf8(isolate, "get_custom_property_vector2d").ToLocalChecked(),
                             v8::FunctionTemplate::New(isolate, &UIBaseControlScriptObject::GetCustomPropertyVector2D));

        controlTemplate->Set(v8::String::NewFromUtf8(isolate, "get_control_by_id").ToLocalChecked(),
                             v8::FunctionTemplate::New(isolate, &UIBaseControlScriptObject::GetControlById));

        controlTemplate->Set(v8::String::NewFromUtf8(isolate, "get_parent_custom_control").ToLocalChecked(),
                             v8::FunctionTemplate::New(isolate, &UIBaseControlScriptObject::GetParentCustomControl));

        controlTemplate->Set(v8::String::NewFromUtf8(isolate, "invoke_javascript_function").ToLocalChecked(),
                             v8::FunctionTemplate::New(isolate, &UIBaseControlScriptObject::InvokeJavascriptFunction));

        UIBaseControlScriptObject::_controlTemplate.Reset(isolate, controlTemplate);
        return UIBaseControlScriptObject::_controlTemplate.Get(isolate);
    }

    void UIBaseControlScriptObject::IdGetter(v8::Local<v8::String>,
                                             const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        info.GetReturnValue().Set(shared::scripting::Script::StringToParameter(control->GetId(),
                                                                               isolate));
    }

    void UIBaseControlScriptObject::IdSetter(v8::Local<v8::String>, v8::Local<v8::Value> value,
                                             const v8::PropertyCallbackInfo<void>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        v8::String::Utf8Value utf8Value(isolate, value);

        control->SetId(*utf8Value);
    }

    void UIBaseControlScriptObject::TextGetter(v8::Local<v8::String>,
                                               const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        info.GetReturnValue().Set(shared::scripting::Script::StringToParameter(control->Script_GetText(),
                                                                               isolate));
    }

    void UIBaseControlScriptObject::TextSetter(v8::Local<v8::String>, v8::Local<v8::Value> value,
                                               const v8::PropertyCallbackInfo<void>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        v8::String::Utf8Value utf8Value(isolate, value);

        control->Script_SetText(*utf8Value);
    }

    void UIBaseControlScriptObject::WidthGetter(v8::Local<v8::String>,
                                                const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        info.GetReturnValue().Set(control->GetSize().GetWidth());
    }

    void UIBaseControlScriptObject::WidthSetter(v8::Local<v8::String>, v8::Local<v8::Value> value,
                                                const v8::PropertyCallbackInfo<void>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        auto size = control->GetSize();
        auto w = value->Int32Value(isolate->GetCurrentContext()).ToChecked();
        size.SetSize(static_cast<uint32_t>(w), size.GetHeight());
        control->SetSize(size);
    }

    void UIBaseControlScriptObject::HeightGetter(v8::Local<v8::String>,
                                                 const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        info.GetReturnValue().Set(control->GetSize().GetHeight());
    }

    void UIBaseControlScriptObject::HeightSetter(v8::Local<v8::String>, v8::Local<v8::Value> value,
                                                 const v8::PropertyCallbackInfo<void>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        auto size = control->GetSize();
        auto h = value->Int32Value(isolate->GetCurrentContext()).ToChecked();
        size.SetSize(size.GetWidth(), static_cast<uint32_t>(h));
        control->SetSize(size);
    }

    void UIBaseControlScriptObject::PosXGetter(v8::Local<v8::String>,
                                               const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        auto pos = control->GetPosition();
        auto [x, _] = pos.GetPosition();
        info.GetReturnValue().Set(x);
    }

    void UIBaseControlScriptObject::PosXSetter(v8::Local<v8::String>, v8::Local<v8::Value> value,
                                               const v8::PropertyCallbackInfo<void>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        auto position = control->GetPosition();
        auto [x, y] = position.GetPosition();

        x = value->Int32Value(isolate->GetCurrentContext()).ToChecked();

        position.SetPositionFixed(x, y);
        control->SetPosition(position);
    }

    void UIBaseControlScriptObject::PosYGetter(v8::Local<v8::String>,
                                               const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        auto pos = control->GetPosition();
        auto [_, y] = pos.GetPosition();
        info.GetReturnValue().Set(y);
    }

    void UIBaseControlScriptObject::PosYSetter(v8::Local<v8::String>, v8::Local<v8::Value> value,
                                               const v8::PropertyCallbackInfo<void>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        auto position = control->GetPosition();
        auto [x, y] = position.GetPosition();

        y = value->Int32Value(isolate->GetCurrentContext()).ToChecked();

        position.SetPositionFixed(x, y);
        control->SetPosition(position);
    }

    void UIBaseControlScriptObject::IsVisibleGetter(v8::Local<v8::String>,
                                                    const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        auto isVisible = control->GetIsVisible();
        info.GetReturnValue().Set(isVisible);
    }

    void UIBaseControlScriptObject::IsVisibleSetter(v8::Local<v8::String>, v8::Local<v8::Value> value,
                                                    const v8::PropertyCallbackInfo<void>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        auto isVisible = value->BooleanValue(isolate);

        control->SetIsVisible(isVisible);
    }

    void UIBaseControlScriptObject::IsEnabledGetter(v8::Local<v8::String>,
                                                    const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        auto isEnabled = control->GetIsEnabled();
        info.GetReturnValue().Set(isEnabled);
    }

    void UIBaseControlScriptObject::IsEnabledSetter(v8::Local<v8::String>, v8::Local<v8::Value> value,
                                                    const v8::PropertyCallbackInfo<void>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        auto isEnabled = value->BooleanValue(isolate);

        control->SetIsEnabled(isEnabled);
    }

    void UIBaseControlScriptObject::GetCustomPropertyString(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        if (args.Length() < 1)
        {
            shared::api::logging::Log("Invalid number of arguments for 'GetCustomPropertyString'.");
            return;
        }

        auto name = shared::scripting::Script::ArgumentToString(isolate, args, 0);

        std::vector<projectfarm::shared::scripting::FunctionParameter> parameters;

        for (auto i = 1; i < args.Length(); ++i)
        {
            auto parameter = shared::scripting::Script::ArgumentToString(isolate, args, i);
            projectfarm::shared::scripting::FunctionParameter functionParameter { parameter };
            parameters.emplace_back(std::move(functionParameter));
        }

        auto result = control->Script_GetCustomPropertyString(name, parameters);

        args.GetReturnValue().Set(shared::scripting::Script::StringToParameter(result, isolate));
    }

    void UIBaseControlScriptObject::GetCustomPropertyInt(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        if (args.Length() < 1)
        {
            shared::api::logging::Log("Invalid number of arguments for 'GetCustomPropertyInt'.");
            return;
        }

        auto name = shared::scripting::Script::ArgumentToString(isolate, args, 0);

        std::vector<projectfarm::shared::scripting::FunctionParameter> parameters;

        for (auto i = 1; i < args.Length(); ++i)
        {
            auto parameter = shared::scripting::Script::ArgumentToString(isolate, args, i);
            projectfarm::shared::scripting::FunctionParameter functionParameter { parameter };
            parameters.emplace_back(std::move(functionParameter));
        }

        auto result = control->Script_GetCustomPropertyInt(name, parameters);

        args.GetReturnValue().Set(result);
    }

    void UIBaseControlScriptObject::GetCustomPropertyBool(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        if (args.Length() < 1)
        {
            shared::api::logging::Log("Invalid number of arguments for 'GetCustomPropertyBool'.");
            return;
        }

        auto name = shared::scripting::Script::ArgumentToString(isolate, args, 0);

        std::vector<projectfarm::shared::scripting::FunctionParameter> parameters;

        for (auto i = 1; i < args.Length(); ++i)
        {
            auto parameter = shared::scripting::Script::ArgumentToString(isolate, args, i);
            projectfarm::shared::scripting::FunctionParameter functionParameter { parameter };
            parameters.emplace_back(std::move(functionParameter));
        }

        auto result = control->Script_GetCustomPropertyBool(name, parameters);

        args.GetReturnValue().Set(result);
    }

    void UIBaseControlScriptObject::GetCustomPropertyVector2D(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        if (args.Length() < 1 || args.Length() > 2)
        {
            shared::api::logging::Log("Invalid number of arguments for 'GetCustomPropertyVector2D'.");
            return;
        }

        auto name = shared::scripting::Script::ArgumentToString(isolate, args, 0);
        auto parameter = args.Length() > 1 ? shared::scripting::Script::ArgumentToString(isolate, args, 1)
                                           : "";

        auto result = new shared::math::ScriptableVector2D();
        result->_object = control->Script_GetCustomPropertyVector2D(name, parameter);

        auto objectInstance = shared::scripting::math::Vector2DScriptObject::
                GetObjectTemplateInstance(isolate, result);

        // as we new up the result above, we need to delete it when it is finalized
        result->SetGCPersistent<shared::math::ScriptableVector2D>(isolate, objectInstance);

        args.GetReturnValue().Set(objectInstance);
    }

    void UIBaseControlScriptObject::GetControlById(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        if (args.Length() != 1)
        {
            shared::api::logging::Log("Invalid number of arguments for 'GetControlById'.");
            return;
        }

        auto id = shared::scripting::Script::ArgumentToString(isolate, args, 0);

        auto foundControl = control->FindControlById(id);

        if (foundControl && *foundControl)
        {
            auto objectInstance = UIBaseControlScriptObject::GetObjectTemplateInstance(isolate, (*foundControl).get());

            args.GetReturnValue().Set(objectInstance);
        }
    }

    void UIBaseControlScriptObject::GetParentCustomControl(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        if (args.Length() != 0)
        {
            shared::api::logging::Log("Invalid number of arguments for 'GetParentCustomControl'.");
            return;
        }

        auto controlToCheck = control->GetParent().get();
        while (controlToCheck && controlToCheck->GetControlType() != graphics::ui::ControlTypes::Custom)
        {
            if (controlToCheck->GetParent())
            {
                controlToCheck = controlToCheck->GetParent().get();
            }
            else
            {
                controlToCheck = nullptr;
                break;
            }
        }

        if (controlToCheck)
        {
            auto objectInstance = UIBaseControlScriptObject::GetObjectTemplateInstance(isolate, controlToCheck);

            args.GetReturnValue().Set(objectInstance);
        }
    }

    void UIBaseControlScriptObject::InvokeJavascriptFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIBaseControlScriptObject::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        if (args.Length() <= 0)
        {
            shared::api::logging::Log("Invalid number of arguments for 'InvokeJavascriptFunction'.");
            return;
        }

        auto functionName = shared::scripting::Script::ArgumentToString(isolate, args, 0);

        std::vector<projectfarm::shared::scripting::FunctionParameter> parameters;

        for (auto i = 1; i < args.Length(); ++i)
        {
            auto parameter = shared::scripting::Script::ArgumentToString(isolate, args, i);
            projectfarm::shared::scripting::FunctionParameter functionParameter { parameter };
            parameters.emplace_back(std::move(functionParameter));
        }

        auto result = control->CallScriptFunction(functionName, parameters);

        args.GetReturnValue().Set(result);
    }
}