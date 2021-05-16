#ifndef PROJECTFARM_PLOT_H
#define PROJECTFARM_PLOT_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "logging/consume_logger.h"
#include "entities/character_states.h"

namespace projectfarm::engine::world
{
    class Plot final : public shared::ConsumeLogger
    {
    public:
        Plot() = default;
        ~Plot() override = default;

        [[nodiscard]] bool LoadFromJson(const nlohmann::json& plotJson) noexcept;

        void SetName(std::string_view name) noexcept
        {
            this->_name = name;
        }

        [[nodiscard]] const std::string& GetName() const noexcept
        {
            return this->_name;
        }

        [[nodiscard]] const std::vector<shared::entities::CharacterStates>& GetDisallowedCharacterStates() const noexcept
        {
            return this->_disallowedCharacterStates;
        }

        void AddDisallowedCharacterState(shared::entities::CharacterStates state) noexcept
        {
            this->_disallowedCharacterStates.push_back(state);
        }

    private:
        std::string _name;

        std::vector<shared::entities::CharacterStates> _disallowedCharacterStates;

        [[nodiscard]] bool SetDisallowedCharacterStates(const nlohmann::json& json) noexcept;
    };
}

#endif
