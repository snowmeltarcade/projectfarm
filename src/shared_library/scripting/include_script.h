#ifndef PROJECTFARM_INCLUDE_SCRIPT_H
#define PROJECTFARM_INCLUDE_SCRIPT_H

namespace projectfarm::shared::scripting
{
    // this is a no-op class used by the script system to create include scripts,
    // but it not actually used itself
    class IncludeScript final : public shared::scripting::Script
    {
    public:
        [[nodiscard]]
        std::vector<std::pair<shared::scripting::FunctionTypes, bool>> GetFunctions() const noexcept override
        {
            return {};
        }

        void SetupGlobalTemplate(v8::Local<v8::ObjectTemplate>&) noexcept override
        {
        }
    };
}

#endif
