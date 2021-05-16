#ifndef PROJECTFARM_SCRIPT_SYSTEM_H
#define PROJECTFARM_SCRIPT_SYSTEM_H

#include <string>
#include <filesystem>
#include <memory>
#include <optional>
#include <v8.h>
#include <libplatform/libplatform.h>

#include "logging/consume_logger.h"
#include "script.h"
#include "script_types.h"
#include "function_types.h"
#include "script_factory.h"
#include "math/consume_random_engine.h"
#include "data/consume_data_provider.h"

namespace projectfarm::shared::scripting
{
    class ScriptSystem final : public shared::ConsumeLogger,
                               public shared::math::ConsumeRandomEngine,
                               public shared::ConsumeDataProvider
    {
    public:
        ScriptSystem() = default;
        ~ScriptSystem() override = default;

        [[nodiscard]]
        bool Initialize(const std::filesystem::path& executableDirectory) noexcept;

        void Shutdown() noexcept;

        void SetScriptFactory(const std::shared_ptr<ScriptFactory>& scriptFactory) noexcept
        {
            this->_scriptFactory = scriptFactory;
        }

        [[nodiscard]]
        std::shared_ptr<Script> CreateScript(ScriptTypes type, const std::filesystem::path& filePath) noexcept;

        [[nodiscard]]
        std::shared_ptr<Script> CreateScript(ScriptTypes type, const std::string& code) noexcept;

        [[nodiscard]]
        static std::string GetFunctionName(FunctionTypes type) noexcept;

        [[nodiscard]]
        static std::optional<v8::Local<v8::Function>> ExtractFunctionFromContextGlobalScope(
                v8::Local<v8::Context>& context,
                const std::string& name) noexcept;

    private:
        std::unique_ptr<v8::Platform> _platform;
        v8::Isolate::CreateParams _createParams;
        v8::Isolate* _isolate {nullptr};

        std::shared_ptr<ScriptFactory> _scriptFactory;

        [[nodiscard]]
        v8::Local<v8::ObjectTemplate> CreateGlobalObjectTemplate(uint8_t numberOfInternalFields) noexcept;

        [[nodiscard]]
        v8::Local<v8::Context> CreateNewScriptContext(const std::shared_ptr<Script>& script) noexcept;

        [[nodiscard]]
        bool CompileCode(const std::string& code,
                         v8::Local<v8::Context>& context,
                         v8::TryCatch& tryCatch) const noexcept;

        [[nodiscard]]
        bool ExtractFunctions(v8::Local<v8::Context>& context,
                              const std::shared_ptr<Script>& script) noexcept;

        [[nodiscard]]
        bool LoadScriptIntoGlobalContext(const std::string& fileName) noexcept;

        static void Log(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void IncludeIntoGlobal(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void RandomInt(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void RandomFloat(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void MathSqrt(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void StringLength(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void StringSubstring(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void StringInsert(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void StringRemoveCharacterAt(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void StringCharAt(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void MarkdownPartPositionToTextPosition(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void TimeUTCShortString(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void TimeUTCLongString(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void TimeLocalTimeShortString(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void TimeLocalTimeLongString(const v8::FunctionCallbackInfo<v8::Value>& args);
    };
}

#endif
