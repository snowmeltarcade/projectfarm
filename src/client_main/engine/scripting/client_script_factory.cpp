#include "ui_control_script.h"
#include "client_script_factory.h"
#include "scripting/include_script.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::scripting
{
    std::shared_ptr<shared::scripting::Script> ClientScriptFactory::CreateScript(shared::scripting::ScriptTypes type) noexcept
    {
        std::shared_ptr<shared::scripting::Script> script;

        switch (type)
        {
            case shared::scripting::ScriptTypes::UIControl:
            {
                script = std::make_shared<UIControlScript>();
                break;
            }
            case shared::scripting::ScriptTypes::Character:
            {
                break;
            }
            case shared::scripting::ScriptTypes::World:
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
            return nullptr;
        }

        return script;
    }
}