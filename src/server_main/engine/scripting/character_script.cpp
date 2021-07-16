#include "character_script.h"
#include "character_script_object.h"
#include "engine/entities/character.h"
#include "engine/world/world.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::scripting
{
    std::vector<std::pair<shared::scripting::FunctionTypes, bool>> CharacterScript::GetFunctions() const noexcept
    {
        return
        {
            { shared::scripting::FunctionTypes::Update, true },
            { shared::scripting::FunctionTypes::Init, true },
        };
    }

    void CharacterScript::SetupGlobalTemplate(v8::Local<v8::ObjectTemplate>& globalTemplate) noexcept
    {
        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "set_update_interval").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &CharacterScript::SetUpdateInterval));

        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "move").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &CharacterScript::Move));

        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "move_to").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &CharacterScript::MoveTo));

        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "get_position_x").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &CharacterScript::GetPositionX));

        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "get_position_y").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &CharacterScript::GetPositionY));

        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "world_get_characters_within_distance").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &CharacterScript::GetCharactersWithinDistance));
    }

    void CharacterScript::SetUpdateInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        using namespace std::literals::string_literals;

        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(1));
        auto character = static_cast<entities::Character*>(wrap->Value());

        if (args.Length() != 1)
        {
            shared::api::logging::Log("Invalid number of arguments for 'SetUpdateInterval'.");
            return;
        }

        auto interval = args[0]->Uint32Value(context).FromMaybe(0);
        if (interval == 0)
        {
            shared::api::logging::Log("Ignoring value - possibly invalid.");
            return;
        }

        character->SetScriptUpdateInterval(interval);
    }

    void CharacterScript::Move(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        using namespace std::literals::string_literals;

        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(1));
        auto character = static_cast<entities::Character*>(wrap->Value());

        if (args.Length() != 4)
        {
            shared::api::logging::Log("Invalid number of arguments for 'Move'.");
            return;
        }

        v8::String::Utf8Value action(isolate, args[0]);

        auto x = args[1]->NumberValue(context).FromMaybe(character->GetLocation().first);
        auto y = args[2]->NumberValue(context).FromMaybe(character->GetLocation().second);

        auto distance = args[3]->NumberValue(context).FromMaybe(0.0f);
        if (distance == 0.0f)
        {
            shared::api::logging::Log("Ignoring distance - possibly invalid.");
            return;
        }

        character->ScriptMove(*action, static_cast<float>(x), static_cast<float>(y),
                                       static_cast<float>(distance));
    }

    void CharacterScript::MoveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        using namespace std::literals::string_literals;

        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(1));
        auto character = static_cast<entities::Character*>(wrap->Value());

        if (args.Length() != 3)
        {
            shared::api::logging::Log("Invalid number of arguments for 'MoveTo'.");
            return;
        }

        v8::String::Utf8Value action(isolate, args[0]);

        auto x = args[1]->NumberValue(context).FromMaybe(0.0f);
        auto y = args[2]->NumberValue(context).FromMaybe(0.0f);

        character->ScriptMoveTo(*action, static_cast<float>(x), static_cast<float>(y));
    }

    void CharacterScript::GetPositionX(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        using namespace std::literals::string_literals;

        auto isolate = args.GetIsolate();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(1));
        auto character = static_cast<entities::Character*>(wrap->Value());

        args.GetReturnValue().Set(character->GetLocation().first);
    }

    void CharacterScript::GetPositionY(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        using namespace std::literals::string_literals;

        auto isolate = args.GetIsolate();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(1));
        auto character = static_cast<entities::Character*>(wrap->Value());

        args.GetReturnValue().Set(character->GetLocation().second);
    }

    void CharacterScript::GetCharactersWithinDistance(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        using namespace std::literals::string_literals;

        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(1));
        auto thisCharacter = static_cast<entities::Character*>(wrap->Value());

        if (args.Length() != 1)
        {
            shared::api::logging::Log("Invalid number of arguments for 'GetCharactersWithinDistance'.");
            return;
        }

        auto distance = static_cast<float>(args[0]->NumberValue(context).FromMaybe(0.0f));
        if (distance == 0.0f)
        {
            auto emptyArray = v8::Array::New(isolate, 0);
            args.GetReturnValue().Set(emptyArray);
            return;
        }

        auto& world = thisCharacter->GetCurrentWorld();

        auto charactersWithinDistance = world->GetCharactersWithinDistance(thisCharacter->GetEntityId(), distance);

        v8::Local<v8::Array> result = v8::Array::New(isolate, static_cast<int>(charactersWithinDistance.size()));

        auto index = 0u;
        for (const auto& character : charactersWithinDistance)
        {
            auto objectInstance = CharacterScriptObject::GetObjectTemplateInstance(isolate, character.get());

            result->Set(context, index++, objectInstance).Check();
        }

        args.GetReturnValue().Set(result);
    }
}