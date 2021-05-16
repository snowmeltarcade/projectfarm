#include "world_script.h"
#include "engine/world/world.h"

namespace projectfarm::engine::scripting
{
    std::vector<std::pair<shared::scripting::FunctionTypes, bool>> WorldScript::GetFunctions() const noexcept
    {
        return
        {
            { shared::scripting::FunctionTypes::Init, true }
        };
    }

    void WorldScript::SetupGlobalTemplate(v8::Local<v8::ObjectTemplate>& globalTemplate) noexcept
    {
        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "get_action_tiles_by_property").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &WorldScript::GetActionTilesByProperty));

        globalTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "add_character").ToLocalChecked(),
                            v8::FunctionTemplate::New(this->_isolate, &WorldScript::AddCharacter));
    }

    void WorldScript::GetActionTilesByProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        using namespace std::literals::string_literals;

        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(1));
        auto world = static_cast<world::World*>(wrap->Value());

        if (args.Length() != 2)
        {
            world->LogMessage("Invalid number of arguments for 'GetActionTilesByPropertyName'.");
            return;
        }

        auto name = Script::ArgumentToString(isolate, args, 0);
        auto value = Script::ArgumentToString(isolate, args, 1);

        auto actionTiles = world->GetActionTilesByProperty(name, value);

        auto rv = v8::Array::New(isolate, static_cast<int>(actionTiles.size()));
        auto index = 0u;

        for (const auto& [islandId, tiles] : actionTiles)
        {
            for (const auto& tile : tiles)
            {
                auto tileObject = v8::Object::New(isolate);
                tileObject->Set(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked(),
                                v8::Number::New(isolate, tile.X)).Check();
                tileObject->Set(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked(),
                                v8::Number::New(isolate, tile.Y)).Check();
                tileObject->Set(context, v8::String::NewFromUtf8(isolate, "islandId").ToLocalChecked(),
                                v8::Number::New(isolate, islandId)).Check();

                rv->Set(context, index++, tileObject).Check();
            }
        }

        args.GetReturnValue().Set(rv);
    }

    void WorldScript::AddCharacter(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        using namespace std::literals::string_literals;

        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(1));
        auto world = static_cast<world::World*>(wrap->Value());

        if (args.Length() < 3 || args.Length() > 4)
        {
            world->LogMessage("Invalid number of arguments for 'AddCharacter'.");
            return;
        }

        auto x = static_cast<uint32_t>(args[0]->NumberValue(context).ToChecked());
        auto y = static_cast<uint32_t>(args[1]->NumberValue(context).ToChecked());

        auto type = Script::ArgumentToString(isolate, args, 2);

        uint8_t islandId = 0u;
        if (args.Length() == 4)
        {
            islandId = static_cast<uint8_t>(args[3]->NumberValue(context).ToChecked());
        }

        auto success = world->AddCharacter(x, y, islandId, type, 0, 0) != nullptr;

        args.GetReturnValue().Set(success);
    }
}