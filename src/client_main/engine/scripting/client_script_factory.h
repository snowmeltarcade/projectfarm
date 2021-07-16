#ifndef PROJECTFARM_CLIENT_SCRIPT_FACTORY_H
#define PROJECTFARM_CLIENT_SCRIPT_FACTORY_H

#include <memory>

#include "scripting/script_factory.h"
#include "scripting/script.h"

namespace projectfarm::engine::scripting
{
    class ClientScriptFactory final : public shared::scripting::ScriptFactory
    {
    public:
        ClientScriptFactory() = default;
        ~ClientScriptFactory() override = default;

        [[nodiscard]]
        std::shared_ptr<shared::scripting::Script> CreateScript(shared::scripting::ScriptTypes type) noexcept override;
    };
}

#endif
