#include "function_parameter.h"

namespace projectfarm::shared::scripting
{
    v8::Local<v8::Value> FunctionParameter::GetAsScriptString(v8::Isolate* isolate) const noexcept
    {
        return v8::String::NewFromUtf8(isolate, this->_value.data()).ToLocalChecked();
    }

    std::optional<v8::Local<v8::Value>> FunctionParameter::GetAsScriptInt(v8::Isolate* isolate) const noexcept
    {
        try
        {
            auto i = std::stoi(this->_value);
            return v8::Int32::New(isolate, i);
        }
        catch (...)
        {
            return {};
        }
    }

    std::optional<v8::Local<v8::Value>> FunctionParameter::GetAsScriptBool(v8::Isolate* isolate) const noexcept
    {
        if (this->_value == "true")
        {
            return v8::Boolean::New(isolate, true);
        }
        else if (this->_value == "false")
        {
            return v8::Boolean::New(isolate, false);
        }

        return {};
    }

    std::string FunctionParameter::GetAsString() const noexcept
    {
        return this->_value;
    }

    std::optional<uint32_t> FunctionParameter::GetAsUInt() const noexcept
    {
        try
        {
            auto i = std::stoi(this->_value);
            return i;
        }
        catch (...)
        {
            return {};
        }
    }

    std::optional<bool> FunctionParameter::GetAsBool() const noexcept
    {
        if (this->_value == "true")
        {
            return true;
        }
        else if (this->_value == "false")
        {
            return false;
        }

        return {};
    }
}