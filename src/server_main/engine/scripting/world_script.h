#ifndef PROJECTFARM_WORLD_SCRIPT_H
#define PROJECTFARM_WORLD_SCRIPT_H

#include <v8.h>

#include "scripting/script.h"
#include "scripting/function_types.h"

namespace projectfarm::engine::scripting
{
    class WorldScript final : public shared::scripting::Script
    {
    public:

        [[nodiscard]]
        std::vector<std::pair<shared::scripting::FunctionTypes, bool>> GetFunctions() const noexcept override;

        void SetupGlobalTemplate(v8::Local<v8::ObjectTemplate>& globalTemplate) noexcept override;

        static void GetActionTilesByProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void AddCharacter(const v8::FunctionCallbackInfo<v8::Value>& args);
    };
}

#endif
