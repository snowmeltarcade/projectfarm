#ifndef PROJECTFARM_SERVER_SCRIPT_FACTORY_H
#define PROJECTFARM_SERVER_SCRIPT_FACTORY_H

#include <memory>

#include "scripting/script_factory.h"
#include "scripting/script.h"
#include "logging/consume_logger.h"

namespace projectfarm::engine::scripting
{
    class ServerScriptFactory final : public shared::scripting::ScriptFactory,
                                      public shared::ConsumeLogger
    {
    public:
        ServerScriptFactory() = default;
        ~ServerScriptFactory() override = default;

        [[nodiscard]] std::shared_ptr<shared::scripting::Script> CreateScript(shared::scripting::ScriptTypes type) noexcept override;
    };
}

#endif
