#include "plot.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::world
{
    bool Plot::LoadFromJson(const nlohmann::json& plotJson) noexcept
    {
        this->_name = plotJson["name"].get<std::string>();

        auto disallowedCharacterStates = plotJson.find("disallowedCharacterStates");
        if (disallowedCharacterStates != plotJson.end())
        {
            if (!this->SetDisallowedCharacterStates(*disallowedCharacterStates))
            {
                shared::api::logging::Log("Failed to set disallowed character states.");
                return false;
            }
        }

        return true;
    }

    bool Plot::SetDisallowedCharacterStates(const nlohmann::json& json) noexcept
    {
        for (const auto& state : json)
        {
            auto s = state.get<std::string>();
            auto characterState = shared::entities::StringToCharacterStates(s);

            this->AddDisallowedCharacterState(characterState);
        }

        return true;
    }
}