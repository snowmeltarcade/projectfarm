#ifndef PROJECTFARM_SCRIPT_FACTORY_H
#define PROJECTFARM_SCRIPT_FACTORY_H

#include <memory>

#include "script.h"
#include "script_types.h"

namespace projectfarm::shared::scripting
{
    class ScriptFactory
    {
    public:
        ScriptFactory() = default;
        virtual ~ScriptFactory() = default;

        [[nodiscard]] virtual std::shared_ptr<Script> CreateScript(ScriptTypes type) noexcept = 0;
    };
}

#endif
