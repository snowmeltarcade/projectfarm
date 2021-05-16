#ifndef PROJECTFARM_PLAYER_H
#define PROJECTFARM_PLAYER_H

#include <memory>

#include <SDL_net.h>

#include "server/client.h"
#include "entities/character.h"
#include "player_load_details.h"

namespace projectfarm::engine::world
{
    class World;
}

namespace projectfarm::engine
{
    class Player final
    {
    public:
        explicit Player(std::shared_ptr<server::Client> networkClient)
        : _networkClient {std::move(networkClient)}
        {}
        ~Player() = default;

        [[nodiscard]]
        const std::shared_ptr<server::Client>& GetNetworkClient() const noexcept
        {
            return this->_networkClient;
        }

        [[nodiscard]]
        const std::shared_ptr<engine::world::World>& GetCurrentWorld() const noexcept
        {
            return this->_currentWorld;
        }

        void SetUDPIPAddress(const IPaddress& ipAddress) noexcept
        {
            this->_udpIPAddress = ipAddress;
        }

        [[nodiscard]]
        const IPaddress& GetUDPIPAddress() const noexcept
        {
            return this->_udpIPAddress;
        }

        void SetCurrentWorld(const std::shared_ptr<engine::world::World>& currentWorld) noexcept
        {
            this->_currentWorld = currentWorld;
        }

        void SetCharacter(const std::shared_ptr<entities::Character>& character) noexcept
        {
            this->_character = character;
        }

        [[nodiscard]]
        const std::shared_ptr<entities::Character>& GetCharacter() const noexcept
        {
            return this->_character;
        }

        [[nodiscard]]
        uint32_t GetPlayerId() const noexcept
        {
            return this->_playerId;
        }

        void SetPlayerId(uint32_t playerId) noexcept
        {
            this->_playerId = playerId;
        }

        [[nodiscard]]
        const std::string& GetUsername() const noexcept
        {
            return this->_username;
        }

        void SetUsername(const std::string& username) noexcept
        {
            this->_username = username;
        }

        void SetLoadDetails(const std::shared_ptr<PlayerLoadDetails>& loadDetails) noexcept;

        [[nodiscard]]
        const std::string& GetCharacterType() const noexcept
        {
            return this->_characterType;
        }

    private:
        std::shared_ptr<server::Client> _networkClient;
        IPaddress _udpIPAddress {};

        std::shared_ptr<engine::world::World> _currentWorld;

        std::shared_ptr<entities::Character> _character;
        std::string _characterType;

        std::string _username;

        static uint32_t GlobalPlayerIdCount;
        uint32_t _playerId = ++Player::GlobalPlayerIdCount; // start the player id at 1
    };
}

#endif
