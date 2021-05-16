#ifndef PROJECTFARM_CHARACTER_SCRIPT_H
#define PROJECTFARM_CHARACTER_SCRIPT_H

#include <vector>

#include <v8.h>

#include "scripting/script.h"
#include "scripting/function_types.h"

namespace projectfarm::engine::scripting
{
    class CharacterScript final : public shared::scripting::Script
    {
    public:
        CharacterScript() = default;
        ~CharacterScript() override = default;

        [[nodiscard]] std::vector<std::pair<shared::scripting::FunctionTypes, bool>>
            GetFunctions() const noexcept override;

        void SetupGlobalTemplate(v8::Local<v8::ObjectTemplate>& globalTemplate) noexcept override;

        static void SetUpdateInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Move(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void MoveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetPositionX(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetPositionY(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void GetCharactersWithinDistance(const v8::FunctionCallbackInfo<v8::Value>& args);
    };
}

#endif
