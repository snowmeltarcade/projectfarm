#include "character_script_object.h"

namespace projectfarm::engine::scripting
{
    v8::Persistent<v8::ObjectTemplate> CharacterScriptObject::_characterTemplate;

    v8::Local<v8::Object> CharacterScriptObject::GetObjectTemplateInstance(v8::Isolate* isolate,
                                                                           entities::Character* character) noexcept
    {
        auto characterTemplate = CharacterScriptObject::GetObjectTemplate(isolate);

        auto context = isolate->GetCurrentContext();

        auto instance = characterTemplate->NewInstance(context).ToLocalChecked();
        instance->SetInternalField(0, v8::External::New(isolate, character));

        return instance;
    }

    v8::Local<v8::ObjectTemplate> CharacterScriptObject::GetObjectTemplate(v8::Isolate* isolate) noexcept
    {
        if (!CharacterScriptObject::_characterTemplate.IsEmpty())
        {
            return CharacterScriptObject::_characterTemplate.Get(isolate);
        }

        auto characterTemplate = v8::ObjectTemplate::New(isolate);

        characterTemplate->SetInternalFieldCount(1);

        characterTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "position_x").ToLocalChecked(),
                                       CharacterScriptObject::PositionXGetter,
                                       CharacterScriptObject::PositionXSetter);

        characterTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "position_y").ToLocalChecked(),
                                       CharacterScriptObject::PositionYGetter,
                                       CharacterScriptObject::PositionYSetter);

        characterTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, "type").ToLocalChecked(),
                                       CharacterScriptObject::TypeGetter,
                                       CharacterScriptObject::TypeSetter);


        CharacterScriptObject::_characterTemplate.Reset(isolate, characterTemplate);
        return CharacterScriptObject::_characterTemplate.Get(isolate);
    }

    void CharacterScriptObject::PositionXGetter(v8::Local<v8::String> /*property*/,
                                                const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto character = static_cast<entities::Character*>(wrap->Value());

        info.GetReturnValue().Set(character->GetLocation().first);
    }

    void CharacterScriptObject::PositionXSetter(v8::Local<v8::String>, v8::Local<v8::Value>,
                                                const v8::PropertyCallbackInfo<void>&)
    {
        // we don't want to set, so just ignore this...
    }

    void CharacterScriptObject::PositionYGetter(v8::Local<v8::String> /*property*/,
                                                const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto character = static_cast<entities::Character*>(wrap->Value());

        info.GetReturnValue().Set(character->GetLocation().second);
    }

    void CharacterScriptObject::PositionYSetter(v8::Local<v8::String>, v8::Local<v8::Value>,
                                                const v8::PropertyCallbackInfo<void>&)
    {
        // we don't want to set, so just ignore this...
    }

    void CharacterScriptObject::TypeGetter(v8::Local<v8::String> /*property*/,
                                           const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto character = static_cast<entities::Character*>(wrap->Value());

        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, character->GetCharacterType().c_str()).ToLocalChecked());
    }

    void CharacterScriptObject::TypeSetter(v8::Local<v8::String>, v8::Local<v8::Value>,
                                           const v8::PropertyCallbackInfo<void>&)
    {
        // we don't want to set, so just ignore this...
    }
}