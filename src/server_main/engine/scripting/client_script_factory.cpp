#include "character_script.h"
#include "server_script_factory.h"
#include "world_script.h"
#include "scripting/include_script.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::scripting
{
    std::shared_ptr<shared::scripting::Script> ServerScriptFactory::CreateScript(shared::scripting::ScriptTypes type) noexcept
    {
        std::shared_ptr<shared::scripting::Script> script;

        switch (type)
        {
            case shared::scripting::ScriptTypes::Character:
            {
                script = std::make_shared<CharacterScript>();
                break;
            }
            case shared::scripting::ScriptTypes::World:
            {
                script = std::make_shared<WorldScript>();
                break;
            }
            case shared::scripting::ScriptTypes::UIControl:
            {
                break;
            }
            case shared::scripting::ScriptTypes::Include:
            {
                script = std::make_shared<shared::scripting::IncludeScript>();
                break;
            }
        }

        if (!script)
        {
            shared::api::logging::Log("Unknown script type: " + std::to_string(static_cast<uint8_t>(type)));
            return {};
        }

        return script;
    }
}