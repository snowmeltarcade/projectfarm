#ifndef PROJECTFARM_DATA_MANAGER_H
#define PROJECTFARM_DATA_MANAGER_H

#include <memory>

#include "data/consume_data_provider.h"
#include "persistence/database.h"
#include "entities/character_appearance_details.h"

namespace projectfarm::shared::entities
{
    struct CharacterAppearanceDetails;
}

namespace projectfarm::engine::data
{
    class DataManager final : public shared::ConsumeDataProvider
    {
    public:
        DataManager() = default;
        ~DataManager() override
        {
            this->Shutdown();
        }

        [[nodiscard]]
        bool Initialize() noexcept;

        void Shutdown() noexcept;

        [[nodiscard]] bool UpdateEntityAppearance(uint32_t entityId,
                                                  const shared::entities::CharacterAppearanceDetails& appearanceDetails,
                                                  bool insert = false) const noexcept;

        [[nodiscard]] bool GetEntityAppearance(uint32_t entityId,
                                               shared::entities::CharacterAppearanceDetails& appearanceDetails) const noexcept;

        [[nodiscard]] bool GetPlayerAppearance(uint32_t playerId,
                                               shared::entities::CharacterAppearanceDetails& appearanceDetails) const noexcept;

        [[nodiscard]] bool InsertPlayer(const std::string& userName,
                                        const std::string& hashedPassword,
                                        uint32_t& playerId) const noexcept;

        [[nodiscard]] bool UpdatePlayerLogin(const std::string& userName,
                                             uint32_t& playerId) const noexcept;

        [[nodiscard]] bool UpdatePlayerCurrentWorld(uint32_t playerId, const std::string& worldName) const noexcept;

        [[nodiscard]] bool UpdatePlayerState(uint32_t playerId, int32_t xPos, int32_t yPos) const noexcept;

        [[nodiscard]] bool UpdatePlayerAppearanceDetails(uint32_t playerId, const shared::entities::CharacterAppearanceDetails& appearanceDetails) const noexcept;

        [[nodiscard]] bool GetHashedPassword(const std::string& userName, std::string& hashedPassword) const noexcept;

        [[nodiscard]] bool GetPlayerLoadDetails(uint32_t playerId, std::string& characterType) const noexcept;

        [[nodiscard]] bool GetPlayerIdByUserName(const std::string& userName, uint32_t& playerId) const noexcept;

        [[nodiscard]] bool GetCurrentWorldByPlayerId(uint32_t playerId, std::string& currentWorld) const noexcept;

        [[nodiscard]] bool GetPosByPlayerId(uint32_t playerId, uint32_t& xPos, uint32_t& yPos) const noexcept;

    private:
        std::shared_ptr<shared::persistence::Database> _serverCacheDatabase;
        std::shared_ptr<shared::persistence::Database> _playerDatabase;

        std::shared_ptr<shared::persistence::Statement> _serverCacheInsertEntity;
        std::shared_ptr<shared::persistence::Statement> _serverCacheUpdateEntity;
        std::shared_ptr<shared::persistence::Statement> _serverCacheGetEntityAppearance;

        std::shared_ptr<shared::persistence::Statement> _playerDatabaseInsertPlayer;

        std::shared_ptr<shared::persistence::Statement> _playerDatabaseUpdatePlayerLogin;
        std::shared_ptr<shared::persistence::Statement> _playerDatabaseUpdatePlayerCurrentWorld;
        std::shared_ptr<shared::persistence::Statement> _playerDatabaseUpdatePlayerState;
        std::shared_ptr<shared::persistence::Statement> _playerDatabaseUpdatePlayerAppearanceDetails;

        std::shared_ptr<shared::persistence::Statement> _playerDatabaseGetHashedPassword;
        std::shared_ptr<shared::persistence::Statement> _playerDatabaseGetPlayerLoadDetails;
        std::shared_ptr<shared::persistence::Statement> _playerDatabaseGetPlayerIdByUserName;
        std::shared_ptr<shared::persistence::Statement> _playerDatabaseGetCurrentWorldByPlayerId;
        std::shared_ptr<shared::persistence::Statement> _playerDatabaseGetPosByPlayerId;
        std::shared_ptr<shared::persistence::Statement> _playerDatabaseGetPlayerAppearance;

        [[nodiscard]] bool SetupPlayerDatabase() noexcept;
        [[nodiscard]] bool SetupServerCacheDatabase() noexcept;

        [[nodiscard]] bool CreateServerCacheStatements() noexcept;
        [[nodiscard]] bool CreatePlayerDatabaseStatements() noexcept;
    };
}

#endif