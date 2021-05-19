#include "scene_script_object.h"
#include "scripting/script.h"

namespace projectfarm::engine::scripting
{
    v8::Persistent<v8::ObjectTemplate> SceneScriptObject::_controlTemplate;

    v8::Local<v8::Object> SceneScriptObject::GetObjectTemplateInstance(v8::Isolate* isolate,
                                                                       scenes::Scene* scene) noexcept
    {
        auto controlTemplate = SceneScriptObject::GetObjectTemplate(isolate);

        auto context = isolate->GetCurrentContext();

        auto instance = controlTemplate->NewInstance(context).ToLocalChecked();
        instance->SetInternalField(SceneScriptObject::InternalFieldIndex_Scene,
                                   v8::External::New(isolate, scene));

        return instance;
    }

    v8::Local<v8::ObjectTemplate> SceneScriptObject::GetObjectTemplate(v8::Isolate* isolate) noexcept
    {
        if (!SceneScriptObject::_controlTemplate.IsEmpty())
        {
            return SceneScriptObject::_controlTemplate.Get(isolate);
        }

        auto controlTemplate = v8::ObjectTemplate::New(isolate);

        controlTemplate->SetInternalFieldCount(1);

        controlTemplate->Set(shared::scripting::Script::StringToParameter("send_message", isolate),
                             v8::FunctionTemplate::New(isolate, &SceneScriptObject::SendMessage));

        SceneScriptObject::_controlTemplate.Reset(isolate, controlTemplate);
        return SceneScriptObject::_controlTemplate.Get(isolate);
    }

    void SceneScriptObject::SendMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                SceneScriptObject::InternalFieldIndex_Scene));
        auto scene = static_cast<scenes::Scene*>(wrap->Value());

        if (args.Length() == 0)
        {
            scene->LogMessage("Invalid number of arguments for 'SendMessage'.");
            return;
        }

        auto key = shared::scripting::Script::ArgumentToString(isolate, args, 0);

        std::vector<projectfarm::shared::scripting::FunctionParameter> parameters;

        for (auto i = 1; i < args.Length(); ++i)
        {
            auto parameter = shared::scripting::Script::ArgumentToString(isolate, args, i);
            projectfarm::shared::scripting::FunctionParameter functionParameter { parameter };
            parameters.emplace_back(std::move(functionParameter));
        }

        auto result = scene->SendUIMessage(key, parameters);

        args.GetReturnValue().Set(shared::scripting::Script::StringToParameter(result, isolate));
    }
}