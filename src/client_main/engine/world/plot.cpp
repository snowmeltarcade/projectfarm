#include "plot.h"

namespace projectfarm::engine::world
{
    bool Plot::LoadFromJson(const nlohmann::json& jsonFile)
    {
        this->_name = jsonFile["name"].get<std::string>();

        auto animationsIter = jsonFile.find("animations");
        if (animationsIter == jsonFile.end() || *animationsIter == nullptr) // `animations` can be null in the json
        {
            if (!this->LoadAnimationFrameFromJson(jsonFile))
            {
                this->LogMessage("Failed to set plot data: " + jsonFile.dump());
                return false;
            }
        }
        else
        {
            for (const auto& animationJson : *animationsIter)
            {
                if (!this->LoadAnimationFrameFromJson(animationJson))
                {
                    this->LogMessage("Failed to load animation data: " + animationJson.dump());
                    return false;
                }
            }
        }

        auto disallowedCharacterStates = jsonFile.find("disallowedCharacterStates");
        if (disallowedCharacterStates != jsonFile.end())
        {
            if (!this->SetDisallowedCharacterStates(*disallowedCharacterStates))
            {
                this->LogMessage("Failed to set disallowed character states.");
                return false;
            }
        }

        return true;
    }

    bool Plot::LoadAnimationFrameFromJson(const nlohmann::json& animationJson)
    {
        auto tileX = animationJson["tileX"].get<uint32_t>();
        auto tileY = animationJson["tileY"].get<uint32_t>();
        auto tileSetName = animationJson["tileSet"].get<std::string>();

        auto millisecondsIter = animationJson.find("milliseconds");
        auto milliseconds = millisecondsIter == animationJson.end() ? 0u : (*millisecondsIter).get<uint32_t>();

        auto tileSet = this->_tileSetPool->Get(tileSetName);
        if (tileSet == nullptr)
        {
            this->LogMessage("Failed to find tileSet with name: " + tileSetName);
            return false;
        }

        auto absoluteIndex = tileSet->GetAbsoluteIndex(tileX, tileY);

        this->AddFrame(milliseconds, absoluteIndex, tileSetName);

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