#include "player.h"

namespace projectfarm::engine
{
    uint32_t Player::GlobalPlayerIdCount = 0;

    void Player::SetLoadDetails(const std::shared_ptr<PlayerLoadDetails>& loadDetails) noexcept
    {
        this->_characterType = loadDetails->CharacterType;
    }
}