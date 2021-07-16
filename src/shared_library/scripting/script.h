#ifndef PROJECTFARM_SCRIPT_H
#define PROJECTFARM_SCRIPT_H

#include <vector>
#include <unordered_map>

#include <v8.h>

#include "function_types.h"
#include "function_parameter.h"

namespace projectfarm::shared::scripting
{
    class Script
    {
    public:
        Script() = default;
        virtual ~Script() = default;

        void SetIsolate(v8::Isolate* isolate) noexcept
        {
            this->_isolate = isolate;
        }

        void SetContext(const v8::Local<v8::Context>& context) noexcept
        {
            this->_context.Reset(this->_isolate, context);
        }

        [[nodiscard]]
        virtual std::vector<std::pair<FunctionTypes, bool>> GetFunctions() const noexcept = 0;

        void SetFunction(FunctionTypes type, v8::Local<v8::Function> function, v8::Isolate* isolate) noexcept
        {
            this->_functions[type].Reset(isolate, function);
        }

        [[nodiscard]]
        bool DoesFunctionExist(FunctionTypes type) noexcept
        {
            return this->_functions.find(type) != this->_functions.end();
        }

        [[nodiscard]]
        bool DoesFunctionExist(const std::string& name) noexcept;

        [[nodiscard]]
        bool CallFunction(FunctionTypes type,
                          const std::vector<FunctionParameter>& parameters) noexcept;

        [[nodiscard]]
        bool CallFunction(const std::string& name,
                          const std::vector<FunctionParameter>& parameters) noexcept;

        void SetObjectInternalField(void* object, uint8_t index = 1) noexcept;

        virtual void SetupGlobalTemplate(v8::Local<v8::ObjectTemplate>&) noexcept
        {
        }

        [[nodiscard]] static std::string ArgumentToString(v8::Isolate* isolate,
                                                          const v8::FunctionCallbackInfo<v8::Value>& args,
                                                          uint32_t index) noexcept;

        [[nodiscard]] static v8::Local<v8::String> StringToParameter(std::string_view s, v8::Isolate* isolate) noexcept
        {
            return v8::String::NewFromUtf8(isolate, s.data()).ToLocalChecked();
        }

        [[nodiscard]] virtual uint8_t GetNumberOfInternalFieldsNeeded() const noexcept
        {
            return 1;
        }

    protected:
        std::unordered_map<FunctionTypes, v8::Persistent<v8::Function>> _functions;

        v8::Isolate* _isolate { nullptr };
        v8::Persistent<v8::Context> _context;

    private:
        [[nodiscard]]
        std::vector<v8::Local<v8::Value>> GetFunctionArguments(const std::vector<FunctionParameter>& parameters) const noexcept;

        [[nodiscard]]
        bool CallFunction(v8::Local<v8::Function> function,
                          const std::vector<FunctionParameter>& parameters) noexcept;
    };
}

#endif
