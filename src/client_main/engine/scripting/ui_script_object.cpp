#include "ui_script_object.h"
#include "ui_base_control_script_object.h"
#include "graphics/ui/ui.h"

namespace projectfarm::engine::scripting
{
    v8::Persistent<v8::ObjectTemplate> UIScriptObject::_controlTemplate;

    v8::Local<v8::Object> UIScriptObject::GetObjectTemplateInstance(v8::Isolate* isolate,
                                                                    graphics::ui::UI* ui) noexcept
    {
        auto controlTemplate = UIScriptObject::GetObjectTemplate(isolate);

        auto context = isolate->GetCurrentContext();

        auto instance = controlTemplate->NewInstance(context).ToLocalChecked();
        instance->SetInternalField(UIScriptObject::InternalFieldIndex_UI,
                                   v8::External::New(isolate, ui));

        return instance;
    }

    v8::Local<v8::ObjectTemplate> UIScriptObject::GetObjectTemplate(v8::Isolate* isolate) noexcept
    {
        if (!UIScriptObject::_controlTemplate.IsEmpty())
        {
            return UIScriptObject::_controlTemplate.Get(isolate);
        }

        auto controlTemplate = v8::ObjectTemplate::New(isolate);

        controlTemplate->SetInternalFieldCount(1);

        controlTemplate->Set(shared::scripting::Script::StringToParameter("get_control_by_id", isolate),
                             v8::FunctionTemplate::New(isolate, &UIScriptObject::GetControlById));

        UIScriptObject::_controlTemplate.Reset(isolate, controlTemplate);
        return UIScriptObject::_controlTemplate.Get(isolate);
    }

    void UIScriptObject::GetControlById(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                UIScriptObject::InternalFieldIndex_UI));
        auto ui = static_cast<graphics::ui::UI*>(wrap->Value());

        if (args.Length() != 1)
        {
            ui->LogMessage("Invalid number of arguments for 'GetControlById'.");
            return;
        }

        auto id = shared::scripting::Script::ArgumentToString(isolate, args, 0);

        auto control = ui->FindControlById(id);

        if (control && *control)
        {
            auto objectInstance = UIBaseControlScriptObject::GetObjectTemplateInstance(isolate, (*control).get());

            args.GetReturnValue().Set(objectInstance);
        }
    }
}