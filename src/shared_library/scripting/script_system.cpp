#include <fstream>
#include <sstream>
#include <cmath>

#include <libplatform/libplatform.h>
#include <v8.h>

#include "script_system.h"
#include "gc_persistent.h"
#include "markdown/markdown.h"
#include "time/clock.h"
#include "api/logging/logging.h"

using namespace std::literals;

namespace projectfarm::shared::scripting
{
    bool ScriptSystem::Initialize(const std::filesystem::path& executableDirectory) noexcept
    {
        api::logging::Log("Initializing v8 with version: "s + v8::V8::GetVersion());

        v8::V8::InitializeICUDefaultLocation(executableDirectory.u8string().c_str());
        // our v8 was compiled not to use external data
        //v8::V8::InitializeExternalStartupData(executableDirectory.u8string().c_str());
        this->_platform = v8::platform::NewDefaultPlatform();
        v8::V8::InitializePlatform(this->_platform.get());
        v8::V8::Initialize();

        this->_createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

#ifdef DEBUG
        v8::V8::SetFlagsFromString("--expose-gc");
#endif

        this->_isolate = v8::Isolate::New(this->_createParams);

        api::logging::Log("Initialized v8.");

        return true;
    }

    void ScriptSystem::Shutdown() noexcept
    {
        api::logging::Log("Shutting down v8...");

        GCPersistent::ClearDeleteQueue();

        if (this->_isolate)
        {
            this->_isolate->Dispose();
            this->_isolate = nullptr;

            // if the isolate is nullptr, this wasn't initialized
            v8::V8::Dispose();
            v8::V8::ShutdownPlatform();
        }

        this->_platform = nullptr;

        if (this->_createParams.array_buffer_allocator)
        {
            delete this->_createParams.array_buffer_allocator;
            this->_createParams.array_buffer_allocator = nullptr;
        }

        api::logging::Log("Shut down v8.");
    }

    std::shared_ptr<Script> ScriptSystem::CreateScript(ScriptTypes type, const std::filesystem::path& filePath) noexcept
    {
        std::ifstream fp(filePath);

        if (!fp.is_open())
        {
            api::logging::Log("Failed to open file: " + filePath.u8string());
            return nullptr;
        }

        std::stringstream ss;
        ss << fp.rdbuf();

        std::string code = ss.str();

        return this->CreateScript(type, code);
    }

    std::shared_ptr<Script> ScriptSystem::CreateScript(ScriptTypes type, const std::string& code) noexcept
    {
        v8::Isolate::Scope isolateScope(this->_isolate);

        v8::HandleScope handleScope(this->_isolate);
        v8::TryCatch tryCatch(this->_isolate);

        auto script = this->_scriptFactory->CreateScript(type);
        if (!script)
        {
            api::logging::Log("Failed to create script of type: " + std::to_string(static_cast<uint8_t>(type)));
            return {};
        }

        script->SetIsolate(this->_isolate);

        auto context = type == ScriptTypes::Include ? this->_isolate->GetCurrentContext()
                                                    : this->CreateNewScriptContext(script);

        v8::Context::Scope contextScope(context);

        if (!this->CompileCode(code, context, tryCatch))
        {
            api::logging::Log("Failed to compile the code: " + code);
            return {};
        }

        if (!this->ExtractFunctions(context, script))
        {
            api::logging::Log("Failed to extract functions.");
            return {};
        }

        script->SetContext(context);

        return script;
    }

    v8::Local<v8::Context> ScriptSystem::CreateNewScriptContext(const std::shared_ptr<Script>& script) noexcept
    {
        auto globalTemplate = this->CreateGlobalObjectTemplate(script->GetNumberOfInternalFieldsNeeded());

        script->SetupGlobalTemplate(globalTemplate);

        auto context = v8::Context::New(this->_isolate, nullptr, globalTemplate);

        auto globalVariableScope = context->Global();

        globalVariableScope->SetInternalField(0, v8::External::New(this->_isolate, this));

        return context;
    }

    bool ScriptSystem::CompileCode(const std::string& code,
                                   v8::Local<v8::Context>& context,
                                   v8::TryCatch& tryCatch) const noexcept
    {
        auto sourceCode = v8::String::NewFromUtf8(this->_isolate, code.c_str()).ToLocalChecked();

        v8::Local<v8::Script> compiledScript;
        if (!v8::Script::Compile(context, sourceCode).ToLocal(&compiledScript))
        {
            v8::String::Utf8Value error(this->_isolate, tryCatch.Exception());

            api::logging::Log("Failed to compile script with error: "s + static_cast<const char*>(*error) +
                             "\nwith code: " + code);
            return false;
        }

        v8::Local<v8::Value> result;
        if (!compiledScript->Run(context).ToLocal(&result))
        {
            v8::String::Utf8Value error(this->_isolate, tryCatch.Exception());

            api::logging::Log("Failed to run script with error: "s + static_cast<const char*>(*error) +
                             "\nwith code: " + code);
            return false;
        }

        // we currently only care about the functions now in the global scope
        // so ignore any actual script results...

        return true;
    }

    bool ScriptSystem::ExtractFunctions(v8::Local<v8::Context>& context,
                                        const std::shared_ptr<Script>& script) noexcept
    {
        v8::HandleScope handleScope(this->_isolate);

        auto functions = script->GetFunctions();

        for (const auto& [type, required] : functions)
        {
            auto functionName = ScriptSystem::GetFunctionName(type);

            auto scriptFunction = this->ExtractFunctionFromContextGlobalScope(context, functionName);
            if (!scriptFunction)
            {
                if (required)
                {
                    api::logging::Log("Failed to find the function: " + functionName);
                    return false;
                }
                else
                {
                    continue;
                }
            }

            script->SetFunction(type, *scriptFunction, this->_isolate);
        }

        return true;
    }

    bool ScriptSystem::LoadScriptIntoGlobalContext(const std::string& fileName) noexcept
    {
        auto filePath = this->_dataProvider->NormalizePath(fileName);

        // ignore the returned include script, as that is just a no-op class,
        // but we do want to ensure `nullptr` is not returned
        if (!this->CreateScript(ScriptTypes::Include, filePath))
        {
            api::logging::Log("Failed to create script with file path: " + filePath.u8string());
            return false;
        }

        return true;
    }

    std::string ScriptSystem::GetFunctionName(FunctionTypes type) noexcept
    {
        switch (type)
        {
            case FunctionTypes::UnknownFunction:
            {
                return "unknown";
            }
            case FunctionTypes::Update:
            {
                return "update";
            }
            case FunctionTypes::Init:
            {
                return "init";
            }
            case FunctionTypes::OnLoad:
            {
                return "onload";
            }
            case FunctionTypes::OnShutdown:
            {
                return "onshutdown";
            }
            case FunctionTypes::OnClick:
            {
                return "onclick";
            }
            case FunctionTypes::OnMouseEnter:
            {
                return "onmouseenter";
            }
            case FunctionTypes::OnMouseLeave:
            {
                return "onmouseleave";
            }
            case FunctionTypes::OnKeyPress:
            {
                return "onkeypress";
            }
            case FunctionTypes::OnKeyPress_Enter:
            {
                return "onkeypress_enter";
            }
            case FunctionTypes::OnDrag:
            {
                return "ondrag";
            }
            case FunctionTypes::SendMessage:
            {
                return "sendmessage";
            }
        }

        return "";
    }

    std::optional<v8::Local<v8::Function>> ScriptSystem::ExtractFunctionFromContextGlobalScope(v8::Local<v8::Context>& context,
                                                                                               const std::string& name) noexcept
    {
        v8::EscapableHandleScope handleScope(context->GetIsolate());

        auto globalVariableScope = context->Global();

        auto utf8Name = v8::String::NewFromUtf8(context->GetIsolate(), name.c_str()).ToLocalChecked();

        v8::Local<v8::Value> scriptFunction;

        if (!globalVariableScope->Get(context, utf8Name).ToLocal(&scriptFunction))
        {
            return {};
        }

        if (!scriptFunction->IsFunction())
        {
            return {};
        }

        return handleScope.Escape(scriptFunction.As<v8::Function>());
    }

    v8::Local<v8::ObjectTemplate> ScriptSystem::CreateGlobalObjectTemplate(uint8_t numberOfInternalFields) noexcept
    {
        auto globalVariableScopeTemplate = v8::ObjectTemplate::New(this->_isolate);

        // we will store `this` in internal field 0
        // field 1 will likely be used for the object associated with this script
        globalVariableScopeTemplate->SetInternalFieldCount(1 + numberOfInternalFields);

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "log").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::Log));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "include_into_global").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::IncludeIntoGlobal));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "random_int").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::RandomInt));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "random_float").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::RandomFloat));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "math_sqrt").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::MathSqrt));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "string_length").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::StringLength));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "string_substring").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::StringSubstring));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "string_insert").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::StringInsert));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "string_remove_character_at").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::StringRemoveCharacterAt));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "string_char_at").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::StringCharAt));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "markdown_part_position_to_text_position").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::MarkdownPartPositionToTextPosition));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "time_utc_short_string").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::TimeUTCShortString));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "time_utc_long_string").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::TimeUTCLongString));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "time_local_time_short_string").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::TimeLocalTimeShortString));

        globalVariableScopeTemplate->Set(v8::String::NewFromUtf8(this->_isolate, "time_local_time_long_string").ToLocalChecked(),
                                         v8::FunctionTemplate::New(this->_isolate, &ScriptSystem::TimeLocalTimeLongString));

        return globalVariableScopeTemplate;
    }

    void ScriptSystem::Log(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::HandleScope handScope(args.GetIsolate());

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        for (int i = 0; i < args.Length(); ++i)
        {
            v8::String::Utf8Value message(args.GetIsolate(), args[i]);
            api::logging::Log(*message);
        }

        args.GetReturnValue().Set(args.Length());
    }

    void ScriptSystem::IncludeIntoGlobal(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::HandleScope handScope(args.GetIsolate());

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 1)
        {
            api::logging::Log("Invalid number of arguments for function: `include_into_global`.");
            return;
        }

        v8::String::Utf8Value fileName(args.GetIsolate(), args[0]);

        if (!ss->LoadScriptIntoGlobalContext(*fileName))
        {
            api::logging::Log("Failed to include script into global: "s + *fileName);
            return;
        }
    }

    void ScriptSystem::RandomInt(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 2)
        {
            api::logging::Log("Invalid number of arguments for 'RandomInt'.");
            return;
        }

        auto min = args[0]->Int32Value(context).FromMaybe(0);
        auto max = args[1]->Int32Value(context).FromMaybe(0);

        auto result = ss->_randomEngine->Next(min, max);

        args.GetReturnValue().Set(result);
    }

    void ScriptSystem::RandomFloat(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 2)
        {
            api::logging::Log("Invalid number of arguments for 'RandomFloat'.");
            return;
        }

        auto min = args[0]->NumberValue(context).FromMaybe(0.0);
        auto max = args[1]->NumberValue(context).FromMaybe(0.0);

        auto result = ss->_randomEngine->Next(min, max);

        args.GetReturnValue().Set(result);
    }

    void ScriptSystem::MathSqrt(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 1)
        {
            api::logging::Log("Invalid number of arguments for 'MathSqrt'.");
            return;
        }

        auto value = args[0]->NumberValue(context).FromMaybe(0.0);

        auto result = std::sqrt(value);

        args.GetReturnValue().Set(result);
    }

    void ScriptSystem::StringLength(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        auto isolate = args.GetIsolate();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 1)
        {
            api::logging::Log("Invalid number of arguments for 'StringLength'.");
            return;
        }

        auto value = Script::ArgumentToString(isolate, args, 0);

        auto result = static_cast<std::uint32_t>(value.length());

        args.GetReturnValue().Set(result);
    }

    void ScriptSystem::StringSubstring(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() < 2 || args.Length() > 3)
        {
            api::logging::Log("Invalid number of arguments for 'StringSubstring'.");
            return;
        }

        auto s = Script::ArgumentToString(isolate, args, 0);
        auto start = static_cast<int32_t>(args[1]->NumberValue(context).FromMaybe(0));
        auto length = args.Length() > 2 ? static_cast<int32_t>(args[2]->NumberValue(context).FromMaybe(0))
                                        : static_cast<int32_t>(s.length() - start); // the remainder of the string

        if (start < 0)
        {
            api::logging::Log("`start` must be greater than 0 for 'StringSubstring'.");
            return;
        }

        if (length < 0)
        {
            api::logging::Log("`length` must be greater than 0 for 'StringSubstring'.");
            return;
        }

        if (static_cast<uint32_t>(start + length) > s.length())
        {
            api::logging::Log("`start` + `length` must be less than the length of the string for 'StringSubstring'.");
            return;
        }

        auto result = s.substr(start, length);

        args.GetReturnValue().Set(Script::StringToParameter(result, isolate));
    }

    void ScriptSystem::StringInsert(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 3)
        {
            api::logging::Log("Invalid number of arguments for 'StringInsert'.");
            return;
        }

        auto s = Script::ArgumentToString(isolate, args, 0);
        auto value = Script::ArgumentToString(isolate, args, 1);
        auto position = static_cast<int32_t>(args[2]->NumberValue(context).FromMaybe(0));

        if (position < 0)
        {
            api::logging::Log("`position` must be greater than 0 for 'StringInsert'.");
            return;
        }

        if (static_cast<uint32_t>(position) > s.length())
        {
            api::logging::Log("`position` must be >= the length of the string for 'StringInsert'.");
            return;
        }

        auto result = s.insert(position, value);

        args.GetReturnValue().Set(Script::StringToParameter(result, isolate));
    }

    void ScriptSystem::StringRemoveCharacterAt(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 2)
        {
            api::logging::Log("Invalid number of arguments for 'StringRemoveCharacterAt'.");
            return;
        }

        auto s = Script::ArgumentToString(isolate, args, 0);
        auto position = static_cast<int32_t>(args[1]->NumberValue(context).FromMaybe(0));

        if (position < 0)
        {
            api::logging::Log("`position` must be >= than 0 for 'StringRemoveCharacterAt'.");
            return;
        }

        if (static_cast<uint32_t>(position) >= s.length())
        {
            api::logging::Log("`position` must be < the length of the string for 'StringRemoveCharacterAt'.");
            return;
        }

        auto result = s.erase(position, 1);

        args.GetReturnValue().Set(Script::StringToParameter(result, isolate));
    }

    void ScriptSystem::StringCharAt(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 2)
        {
            api::logging::Log("Invalid number of arguments for 'StringCharAt'.");
            return;
        }

        auto s = Script::ArgumentToString(isolate, args, 0);
        auto position = static_cast<int32_t>(args[1]->NumberValue(context).FromMaybe(0));

        if (position < 0)
        {
            api::logging::Log("`position` must be >= than 0 for 'StringCharAt'.");
            return;
        }

        if (static_cast<uint32_t>(position) >= s.length())
        {
            api::logging::Log("`position` must be < the length of the string for 'StringCharAt'.");
            return;
        }

        // we want a string, not a char
        auto result = std::string { s[position] };

        args.GetReturnValue().Set(Script::StringToParameter(result, isolate));
    }

    void ScriptSystem::MarkdownPartPositionToTextPosition(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();
        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 2)
        {
            api::logging::Log("Invalid number of arguments for 'MarkdownPartPositionToTextPosition'.");
            return;
        }

        auto s = Script::ArgumentToString(isolate, args, 0);
        auto position = static_cast<int32_t>(args[1]->NumberValue(context).FromMaybe(0));

        if (position < 0)
        {
            api::logging::Log("`position` must be greater than 0 for 'MarkdownPartPositionToTextPosition'.");
            return;
        }

        if (static_cast<uint32_t>(position) > s.length())
        {
            api::logging::Log("`position` must be <= the length of the string for 'MarkdownPartPositionToTextPosition'.");
            return;
        }

        auto result = projectfarm::shared::markdown::GetMarkdownPositionFromPartPosition(s, position);

        args.GetReturnValue().Set(result);
    }

    void ScriptSystem::TimeUTCShortString(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 0)
        {
            api::logging::Log("Invalid number of arguments for 'TimeUTCShortString'.");
            return;
        }

        auto time = shared::time::Clock::UTCAsShortString();

        args.GetReturnValue().Set(Script::StringToParameter(time, isolate));
    }

    void ScriptSystem::TimeUTCLongString(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 0)
        {
            api::logging::Log("Invalid number of arguments for 'TimeUTCLongString'.");
            return;
        }

        auto time = shared::time::Clock::UTCAsLongString();

        args.GetReturnValue().Set(Script::StringToParameter(time, isolate));
    }

    void ScriptSystem::TimeLocalTimeShortString(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 0)
        {
            api::logging::Log("Invalid number of arguments for 'TimeLocalTimeShortString'.");
            return;
        }

        auto time = shared::time::Clock::LocalTimeAsShortString();

        args.GetReturnValue().Set(Script::StringToParameter(time, isolate));
    }

    void ScriptSystem::TimeLocalTimeLongString(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();
        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        auto ss = static_cast<ScriptSystem*>(wrap->Value());

        if (args.Length() != 0)
        {
            api::logging::Log("Invalid number of arguments for 'TimeLocalTimeLongString'.");
            return;
        }

        auto time = shared::time::Clock::LocalTimeAsLongString();

        args.GetReturnValue().Set(Script::StringToParameter(time, isolate));
    }
}
