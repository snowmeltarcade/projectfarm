#include "ui_control_script.h"
#include "ui_base_control_script_object.h"
#include "ui_script_object.h"
#include "scene_script_object.h"
#include "keyboard_input_script_object.h"
#include "graphics/ui/ui.h"
#include "graphics/ui/base_control.h"
#include "engine/keyboard_input.h"
#include "graphics/ui/control_types.h"

namespace projectfarm::engine::scripting
{
    std::vector<std::pair<shared::scripting::FunctionTypes, bool>> UIControlScript::GetFunctions() const noexcept
    {
        return
        {
            { shared::scripting::FunctionTypes::OnLoad, false },
            { shared::scripting::FunctionTypes::OnShutdown, false },
            { shared::scripting::FunctionTypes::OnClick, false },
            { shared::scripting::FunctionTypes::OnMouseEnter, false },
            { shared::scripting::FunctionTypes::OnMouseLeave, false },
            { shared::scripting::FunctionTypes::OnKeyPress, false },
            { shared::scripting::FunctionTypes::OnKeyPress_Enter, false },
            { shared::scripting::FunctionTypes::OnDrag, false },
            { shared::scripting::FunctionTypes::SendMessage, false },
        };
    }

    void UIControlScript::SetupGlobalTemplate(v8::Local<v8::ObjectTemplate>& globalTemplate) noexcept
    {
        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "get_current_control").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &UIControlScript::GetCurrentControl));

        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "get_ui").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &UIControlScript::GetUI));

        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "get_scene").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &UIControlScript::GetScene));

        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "get_keyboard_input").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &UIControlScript::GetKeyboardInput));
    }

    void UIControlScript::GetCurrentControl(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIControlScript::InternalFieldIndex_Control));
        auto control = static_cast<graphics::ui::BaseControl*>(wrap->Value());

        if (args.Length() != 0)
        {
            control->LogMessage("Invalid number of arguments for 'GetCurrentControl'.");
            return;
        }

        auto objectInstance = UIBaseControlScriptObject::GetObjectTemplateInstance(isolate, control);

        args.GetReturnValue().Set(objectInstance);
    }

    void UIControlScript::GetUI(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIControlScript::InternalFieldIndex_UI));
        auto ui = static_cast<graphics::ui::UI*>(wrap->Value());

        if (args.Length() != 0)
        {
            ui->LogMessage("Invalid number of arguments for 'GetUI'.");
            return;
        }

        auto objectInstance = UIScriptObject::GetObjectTemplateInstance(isolate, ui);

        args.GetReturnValue().Set(objectInstance);
    }

    void UIControlScript::GetScene(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIControlScript::InternalFieldIndex_Scene));
        auto scene = static_cast<scenes::Scene*>(wrap->Value());

        if (args.Length() != 0)
        {
            scene->LogMessage("Invalid number of arguments for 'GetScene'.");
            return;
        }

        auto objectInstance = SceneScriptObject::GetObjectTemplateInstance(isolate, scene);

        args.GetReturnValue().Set(objectInstance);
    }

    void UIControlScript::GetKeyboardInput(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIControlScript::InternalFieldIndex_KeyboardInput));
        auto keyboardInput = static_cast<engine::KeyboardInput*>(wrap->Value());

        if (args.Length() != 0)
        {
            keyboardInput->LogMessage("Invalid number of arguments for 'GetKeyboardInput'.");
            return;
        }

        auto objectInstance = KeyboardInputScriptObject::GetObjectTemplateInstance(isolate, keyboardInput);

        args.GetReturnValue().Set(objectInstance);
    }
}