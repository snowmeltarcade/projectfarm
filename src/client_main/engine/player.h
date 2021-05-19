#ifndef PROJECTFARM_PLAYER_H
#define PROJECTFARM_PLAYER_H

#include <cstdint>
#include <memory>

#include "engine/entities/character.h"

namespace projectfarm::engine
{
    class Player final
    {
    public:
        Player() = default;
        ~Player() = default;

        [[nodiscard]] uint32_t GetPlayerId() const noexcept
        {
            return this->_playerId;
        }

        void SetPlayerId(uint32_t playerId) noexcept
        {
            this->_playerId = playerId;
        }

        [[nodiscard]] const std::shared_ptr<engine::entities::Character>& GetCharacter() const noexcept
        {
            return this->_character;
        }

        void SetCharacter(const std::shared_ptr<engine::entities::Character>& character) noexcept
        {
            this->_character = character;
        }

    private:
        uint32_t _playerId {0};

        std::shared_ptr<engine::entities::Character> _character;
    };
}

#endif
