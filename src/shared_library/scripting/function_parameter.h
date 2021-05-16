#ifndef PROJECTFARM_FUNCTION_PARAMETER_H
#define PROJECTFARM_FUNCTION_PARAMETER_H

#include <string>
#include <optional>
#include <v8.h>

#include "function_parameter_types.h"

namespace projectfarm::shared::scripting
{
    class FunctionParameter
    {
    public:
        explicit FunctionParameter(std::string value)
            : _type(FunctionParameterTypes::String), _value(std::move(value))
        {}

        FunctionParameter(FunctionParameterTypes type, std::string value)
            : _type(type), _value(std::move(value))
        {}

        ~FunctionParameter() = default;

        [[nodiscard]]
        v8::Local<v8::Value> GetAsScriptString(v8::Isolate* isolate) const noexcept;

        [[nodiscard]]
        std::optional<v8::Local<v8::Value>> GetAsScriptInt(v8::Isolate* isolate) const noexcept;

        [[nodiscard]]
        std::string GetAsString() const noexcept;

        [[nodiscard]]
        std::optional<uint32_t> GetAsUInt() const noexcept;

        [[nodiscard]]
        FunctionParameterTypes GetType() const noexcept
        {
            return this->_type;
        }

        [[nodiscard]]
        const std::string& GetValue() const noexcept
        {
            return this->_value;
        }

    private:
        FunctionParameterTypes _type {FunctionParameterTypes::String};
        std::string _value;
    };
}

#endif