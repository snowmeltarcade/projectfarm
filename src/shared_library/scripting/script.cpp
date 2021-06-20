#include "script.h"
#include "script_system.h"

using namespace std::literals;

namespace projectfarm::shared::scripting
{
    bool Script::DoesFunctionExist(const std::string& name) noexcept
    {
        v8::Isolate::Scope isolateScope(this->_isolate);

        auto context = this->_context.Get(this->_isolate);
        v8::Context::Scope contextScope(context);

        if (auto exists = ScriptSystem::ExtractFunctionFromContextGlobalScope(context, name);
            exists)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Script::CallFunction(FunctionTypes type,
                              const std::vector<FunctionParameter>& parameters) noexcept
    {
        if (this->_functions.find(type) == this->_functions.end())
        {
            this->LogMessage("Failed to find function: " + std::to_string(static_cast<uint8_t>(type)));
            return false;
        }

        v8::HandleScope handleScope(this->_isolate);

        auto function = this->_functions[type].Get(this->_isolate);

        if (!this->CallFunction(function, parameters))
        {
            this->LogMessage("Failed to call function: "s + std::to_string(static_cast<uint8_t>(type)));
            return false;
        }

        return true;
    }

    bool Script::CallFunction(const std::string& name,
                              const std::vector<FunctionParameter>& parameters) noexcept
    {
        v8::Isolate::Scope isolateScope(this->_isolate);

        auto context = this->_context.Get(this->_isolate);
        v8::Context::Scope contextScope(context);

        if (auto function = ScriptSystem::ExtractFunctionFromContextGlobalScope(context, name);
            function)
        {
            if (!this->CallFunction(*function, parameters))
            {
                this->LogMessage("Failed to call function: "s + name);
                return false;
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    bool Script::CallFunction(v8::Local<v8::Function> function,
                              const std::vector<FunctionParameter>& parameters) noexcept
    {
        v8::Isolate::Scope isolateScope(this->_isolate);

        auto handleScope = v8::HandleScope(this->_isolate);
        auto tryCatch = v8::TryCatch(this->_isolate);

        auto context = this->_context.Get(this->_isolate);
        v8::Context::Scope contextScope(context);

        auto globalVariableScope = context->Global();

        auto args = this->GetFunctionArguments(parameters);

        v8::Local<v8::Value> functionResult;
        auto callFunctionResult = function->Call(context, globalVariableScope, static_cast<int>(args.size()), args.data());
        if (!callFunctionResult.ToLocal(&functionResult))
        {
            auto s = function->GetName()->ToString(context).FromMaybe(v8::String::NewFromUtf8(this->_isolate, "").ToLocalChecked());
            v8::String::Utf8Value functionName(this->_isolate, s);

            v8::String::Utf8Value error(this->_isolate, tryCatch.Exception());
            this->LogMessage("Failed to run function: "s + *functionName +
                             " with error: " + *error);
            return false;
        }

        // ignore return results at the moment
        //v8::String::Utf8Value frs(this->_isolate, functionResult);
        //this->LogMessage(*frs);

        return true;
    }

    void Script::SetObjectInternalField(void* object, uint8_t index) noexcept
    {
        auto handleScope = v8::HandleScope(this->_isolate);

        auto context = this->_context.Get(this->_isolate);

        auto globalScope = context->Global();

        globalScope->SetInternalField(index, v8::External::New(this->_isolate, object));
    }

    std::string Script::ArgumentToString(v8::Isolate* isolate,
                                         const v8::FunctionCallbackInfo<v8::Value>& args,
                                         uint32_t index) noexcept
    {
        auto context = isolate->GetCurrentContext();

        auto arg = args[index]->ToString(context).FromMaybe(v8::String::NewFromUtf8(isolate, "").ToLocalChecked());
        v8::String::Utf8Value argValue(isolate, arg);
        auto value = *argValue;

        return value;
    }

    std::vector<v8::Local<v8::Value>> Script::GetFunctionArguments(const std::vector<FunctionParameter>& parameters) const noexcept
    {
        std::vector<v8::Local<v8::Value>> args;

        for (const auto& parameter : parameters)
        {
            v8::Local<v8::Value> arg;

            if (parameter.GetType() == FunctionParameterTypes::String)
            {
                arg = parameter.GetAsScriptString(this->_isolate);
            }
            else
            {
                if (auto i = parameter.GetAsScriptInt(this->_isolate); i)
                {
                    arg = *i;
                }
                else if (auto b = parameter.GetAsScriptBool(this->_isolate); b)
                {
                    arg = *b;
                }
                else
                {
                    this->LogMessage("Invalid parameter. Expecting int: " + parameter.GetValue());
                }
            }

            args.emplace_back(std::move(arg));
        }

        return args;
    }
}