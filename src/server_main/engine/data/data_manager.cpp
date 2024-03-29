#include "data_manager.h"
#include "entities/character_appearance_details.h"
#include "platform/platform_id.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::data
{
    bool DataManager::Initialize() noexcept
    {
        shared::api::logging::Log("Initializing data manager...");

        this->_playerDatabase = std::make_shared<shared::persistence::Database>();

        this->_serverCacheDatabase = std::make_shared<shared::persistence::Database>();

        if (!this->SetupPlayerDatabase())
        {
            shared::api::logging::Log("Failed to setup player database.");
            return false;
        }

        if (!this->SetupServerCacheDatabase())
        {
            shared::api::logging::Log("Failed to setup server cache database.");
            return false;
        }

        shared::api::logging::Log("Initialized data manager.");

        return true;
    }

    void DataManager::Shutdown() noexcept
    {
        shared::api::logging::Log("Shutting down data manager...");

        if (this->_serverCacheDatabase && !this->_serverCacheDatabase->Shutdown())
        {
            shared::api::logging::Log("Failed to shut down server cache database.");
            return;
        }

        if (this->_playerDatabase && !this->_playerDatabase->Shutdown())
        {
            shared::api::logging::Log("Failed to shut down player database.");
            return;
        }

        shared::api::logging::Log("Shut down data manager.");
    }

    bool DataManager::SetupPlayerDatabase() noexcept
    {
        shared::api::logging::Log("Setting up player database...");

        // iOS does not give write access to the application bundle directory, so we need to leave the
        // `data` and application directories and go into the `Library` directory
        // https://developer.apple.com/library/archive/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/FileSystemOverview/FileSystemOverview.html
#ifdef IS_IOS
        auto databasePath = std::getenv("HOME") / std::filesystem::path("Library") / "player.db";
#else
        auto databasePath = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::ServerDatabases,
                                                                 "player.db");
#endif
        if (!this->_playerDatabase->Open(databasePath, false))
        {
            shared::api::logging::Log("Failed to open the player database.");
            return false;
        }

        auto createPath = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::ServerDatabases,
                                                               "create_player_database.sql");
        if (!this->_playerDatabase->RunSQLFromFile(createPath))
        {
            shared::api::logging::Log("Failed to run sql file: " + createPath.u8string());
            return false;
        }

        if (!this->CreatePlayerDatabaseStatements())
        {
            shared::api::logging::Log("Failed to create player database statements.");
            return false;
        }

        shared::api::logging::Log("Set up player database.");

        return true;
    }

    bool DataManager::SetupServerCacheDatabase() noexcept
    {
        shared::api::logging::Log("Setting up server cache database...");

        auto templatePath = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::ServerDatabases,
                                                                 "create_server_cache.sql");

        if (!this->_serverCacheDatabase->Open(templatePath, true))
        {
            shared::api::logging::Log("Failed to open server cache database.");
            return false;
        }

        if (!this->CreateServerCacheStatements())
        {
            shared::api::logging::Log("Failed to create server cache statements.");
            return false;
        }

        shared::api::logging::Log("Set up server cache database.");

        return true;
    }

    bool DataManager::CreateServerCacheStatements() noexcept
    {
        auto path = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::ServerDatabases,
                                                         "server_cache_insert_entity.sql");
        if (this->_serverCacheInsertEntity = this->_serverCacheDatabase->CreateStatementFromFile(path);
            !this->_serverCacheInsertEntity)
        {
            shared::api::logging::Log("Failed to create statement from path: " + path.u8string());
            return false;
        }

        path = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::ServerDatabases,
                                                    "server_cache_update_entity.sql");
        if (this->_serverCacheUpdateEntity = this->_serverCacheDatabase->CreateStatementFromFile(path);
            !this->_serverCacheUpdateEntity)
        {
            shared::api::logging::Log("Failed to create statement from path: " + path.u8string());
            return false;
        }

        path = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::ServerDatabases,
                                                    "server_cache_get_entity_appearance.sql");
        if (this->_serverCacheGetEntityAppearance = this->_serverCacheDatabase->CreateStatementFromFile(path);
            !this->_serverCacheGetEntityAppearance)
        {
            shared::api::logging::Log("Failed to create statement from path: " + path.u8string());
            return false;
        }

        return true;
    }

    bool DataManager::UpdateEntityAppearance(uint32_t entityId,
                                             const shared::entities::CharacterAppearanceDetails& appearanceDetails,
                                             bool insert) const noexcept
    {
        uint8_t index = 0u;

        auto statement = insert ? this->_serverCacheInsertEntity : this->_serverCacheUpdateEntity;

        if (!statement->SetParameterInt(index++, entityId))
        {
            shared::api::logging::Log("Failed to set entity id.");
            return false;
        }

        if (!statement->SetParameterString(index++, appearanceDetails.Hair))
        {
            shared::api::logging::Log("Failed to set appearance hair.");
            return false;
        }

        if (!statement->SetParameterString(index++, appearanceDetails.Body))
        {
            shared::api::logging::Log("Failed to set appearance body.");
            return false;
        }

        if (!statement->SetParameterString(index++, appearanceDetails.ClothesTop))
        {
            shared::api::logging::Log("Failed to set appearance clothes top.");
            return false;
        }

        if (!statement->SetParameterString(index++, appearanceDetails.ClothesBottom))
        {
            shared::api::logging::Log("Failed to set appearance clothes bottom.");
            return false;
        }

        if (!statement->SetParameterString(index++, appearanceDetails.Feet))
        {
            shared::api::logging::Log("Failed to set appearance feet.");
            return false;
        }

        if (!statement->Run())
        {
            shared::api::logging::Log("Failed to run server cache insert entity statement.");
            return false;
        }

        return true;
    }

    bool DataManager::GetPlayerAppearance(uint32_t playerId,
                                          shared::entities::CharacterAppearanceDetails& appearanceDetails) const noexcept
    {
        if (!this->_playerDatabaseGetPlayerAppearance->SetParameterInt(0, playerId))
        {
            shared::api::logging::Log("Failed to set player id.");
            return false;
        }

        if (!this->_playerDatabaseGetPlayerAppearance->Run())
        {
            shared::api::logging::Log("Failed to run player database get player appearance statement.");
            return false;
        }

        auto results = this->_playerDatabaseGetPlayerAppearance->GetStringReturnValues();
        if (results.size() != 5)
        {
            shared::api::logging::Log("Incorrect number of results returned for player database get player appearance statement: " +
                             std::to_string(results.size()));
            return false;
        }

        appearanceDetails =
        {
            results[0],
            results[1],
            results[2],
            results[3],
            results[4],
        };

        return true;
    }

    bool DataManager::GetEntityAppearance(uint32_t entityId,
                                          shared::entities::CharacterAppearanceDetails& appearanceDetails) const noexcept
    {
        if (!this->_serverCacheGetEntityAppearance->SetParameterInt(0, entityId))
        {
            shared::api::logging::Log("Failed to set entity id.");
            return false;
        }

        if (!this->_serverCacheGetEntityAppearance->Run())
        {
            shared::api::logging::Log("Failed to run server cache get entity appearance statement.");
            return false;
        }

        auto results = this->_serverCacheGetEntityAppearance->GetStringReturnValues();
        if (results.size() != 5)
        {
            shared::api::logging::Log("Incorrect number of results returned for get entity appearance statement: " +
                             std::to_string(results.size()));
            return false;
        }

        appearanceDetails =
        {
            results[0],
            results[1],
            results[2],
            results[3],
            results[4],
        };

        return true;
    }

    bool DataManager::CreatePlayerDatabaseStatements() noexcept
    {
#define LOAD_SCRIPT(statement, filename) \
        { \
            auto path = this->_dataProvider->ResolveFileName(shared::DataProviderLocations::ServerDatabases, \
                                                             (filename)); \
            if ((statement) = this->_playerDatabase->CreateStatementFromFile(path); \
                !(statement)) \
            { \
                shared::api::logging::Log("Failed to create statement from path: " + path.u8string()); \
                return false; \
            } \
        }

        LOAD_SCRIPT(this->_playerDatabaseInsertPlayer, "player_database_insert_player.sql")

        LOAD_SCRIPT(this->_playerDatabaseUpdatePlayerLogin, "player_database_update_player_login.sql")
        LOAD_SCRIPT(this->_playerDatabaseUpdatePlayerCurrentWorld, "player_database_update_player_current_world.sql")
        LOAD_SCRIPT(this->_playerDatabaseUpdatePlayerState, "player_database_update_player_state.sql")
        LOAD_SCRIPT(this->_playerDatabaseUpdatePlayerAppearanceDetails, "player_database_update_player_appearance_details.sql")

        LOAD_SCRIPT(this->_playerDatabaseGetHashedPassword, "player_database_get_hashed_password.sql")
        LOAD_SCRIPT(this->_playerDatabaseGetPlayerIdByUserName, "player_database_get_playerid_by_username.sql")
        LOAD_SCRIPT(this->_playerDatabaseGetPlayerLoadDetails, "player_database_get_player_load_details.sql")
        LOAD_SCRIPT(this->_playerDatabaseGetCurrentWorldByPlayerId, "player_database_get_current_world_by_playerid.sql")
        LOAD_SCRIPT(this->_playerDatabaseGetPosByPlayerId, "player_database_get_pos_by_playerid.sql")
        LOAD_SCRIPT(this->_playerDatabaseGetPlayerAppearance, "player_database_get_player_appearance.sql")

#undef LOAD_SCRIPT

        return true;
    }

    bool DataManager::InsertPlayer(const std::string& userName,
                                   const std::string& hashedPassword,
                                   uint32_t& playerId) const noexcept
    {
        if (!this->_playerDatabaseInsertPlayer->SetParameterString(0, userName))
        {
            shared::api::logging::Log("Failed to set username.");
            return false;
        }

        if (!this->_playerDatabaseInsertPlayer->SetParameterString(1, hashedPassword))
        {
            shared::api::logging::Log("Failed to set hashed password.");
            return false;
        }

        auto secondsSinceEpoch = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        if (!this->_playerDatabaseInsertPlayer->SetParameterInt(2, secondsSinceEpoch))
        {
            shared::api::logging::Log("Failed to set login time.");
            return false;
        }

        if (!this->_playerDatabaseInsertPlayer->Run())
        {
            shared::api::logging::Log("Failed to run player database insert player statement.");
            return false;
        }

        auto results = this->_playerDatabaseInsertPlayer->GetIntReturnValues();
        if (results.size() != 1)
        {
            shared::api::logging::Log("Incorrect number of results returned for insert player statement: " +
                             std::to_string(results.size()));
            return false;
        }

        playerId = results[0];

        return true;
    }

    bool DataManager::UpdatePlayerLogin(const std::string& userName,
                                        uint32_t& playerId) const noexcept
    {
        if (!this->_playerDatabaseUpdatePlayerLogin->SetParameterString(0, userName))
        {
            shared::api::logging::Log("Failed to set username.");
            return false;
        }

        auto secondsSinceEpoch = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        if (!this->_playerDatabaseUpdatePlayerLogin->SetParameterInt(1, secondsSinceEpoch))
        {
            shared::api::logging::Log("Failed to set login time.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerLogin->Run())
        {
            shared::api::logging::Log("Failed to run player database update player login statement.");
            return false;
        }

        auto results = this->_playerDatabaseUpdatePlayerLogin->GetIntReturnValues();
        if (results.size() != 1)
        {
            shared::api::logging::Log("Incorrect number of results returned for update player login statement: " +
                             std::to_string(results.size()));
            return false;
        }

        playerId = results[0];

        return true;
    }

    bool DataManager::UpdatePlayerCurrentWorld(uint32_t playerId, const std::string& worldName) const noexcept
    {
        if (!this->_playerDatabaseUpdatePlayerCurrentWorld->SetParameterInt(0, playerId))
        {
            shared::api::logging::Log("Failed to set player id.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerCurrentWorld->SetParameterString(1, worldName))
        {
            shared::api::logging::Log("Failed to set world name.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerCurrentWorld->Run())
        {
            shared::api::logging::Log("Failed to run player database update player current world statement.");
            return false;
        }

        return true;
    }

    bool DataManager::UpdatePlayerState(uint32_t playerId, int32_t xPos, int32_t yPos) const noexcept
    {
        if (!this->_playerDatabaseUpdatePlayerState->SetParameterInt(0, playerId))
        {
            shared::api::logging::Log("Failed to set player id.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerState->SetParameterInt(1, xPos))
        {
            shared::api::logging::Log("Failed to set x pos.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerState->SetParameterInt(2, yPos))
        {
            shared::api::logging::Log("Failed to set y pos.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerState->Run())
        {
            shared::api::logging::Log("Failed to run player database update player state statement.");
            return false;
        }

        return true;
    }

    bool DataManager::UpdatePlayerAppearanceDetails(uint32_t playerId,
                                                    const shared::entities::CharacterAppearanceDetails& appearanceDetails) const noexcept
    {
        if (!this->_playerDatabaseUpdatePlayerAppearanceDetails->SetParameterInt(0, playerId))
        {
            shared::api::logging::Log("Failed to set player id.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerAppearanceDetails->SetParameterString(1, appearanceDetails.Hair))
        {
            shared::api::logging::Log("Failed to set hair.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerAppearanceDetails->SetParameterString(2, appearanceDetails.Body))
        {
            shared::api::logging::Log("Failed to set body.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerAppearanceDetails->SetParameterString(3, appearanceDetails.ClothesTop))
        {
            shared::api::logging::Log("Failed to set clothes top.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerAppearanceDetails->SetParameterString(4, appearanceDetails.ClothesBottom))
        {
            shared::api::logging::Log("Failed to set clothes bottom.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerAppearanceDetails->SetParameterString(5, appearanceDetails.Feet))
        {
            shared::api::logging::Log("Failed to set feet.");
            return false;
        }

        if (!this->_playerDatabaseUpdatePlayerAppearanceDetails->Run())
        {
            shared::api::logging::Log("Failed to run player database update player appearance details statement.");
            return false;
        }

        return true;
    }

    bool DataManager::GetHashedPassword(const std::string& userName, std::string& hashedPassword) const noexcept
    {
        if (!this->_playerDatabaseGetHashedPassword->SetParameterString(0, userName))
        {
            shared::api::logging::Log("Failed to set username.");
            return false;
        }

        if (!this->_playerDatabaseGetHashedPassword->Run())
        {
            shared::api::logging::Log("Failed to run player database get hashed password statement.");
            return false;
        }

        auto results = this->_playerDatabaseGetHashedPassword->GetStringReturnValues();
        if (results.size() > 1)
        {
            shared::api::logging::Log("Incorrect number of results returned for get hashed password statement: " +
                             std::to_string(results.size()));
            return false;
        }

        hashedPassword = results.empty() ? "" : results[0];

        return true;
    }

    bool DataManager::GetPlayerLoadDetails(uint32_t playerId, std::string& characterType) const noexcept
    {
        if (!this->_playerDatabaseGetPlayerLoadDetails->SetParameterInt(0, playerId))
        {
            shared::api::logging::Log("Failed to set player id.");
            return false;
        }

        if (!this->_playerDatabaseGetPlayerLoadDetails->Run())
        {
            shared::api::logging::Log("Failed to run player database get player load details statement.");
            return false;
        }

        auto results = this->_playerDatabaseGetPlayerLoadDetails->GetStringReturnValues();
        if (results.size() != 1)
        {
            shared::api::logging::Log("Incorrect number of results returned for get player load details statement: " +
                             std::to_string(results.size()));
            return false;
        }

        characterType = results[0];

        return true;
    }

    bool DataManager::GetPlayerIdByUserName(const std::string& userName, uint32_t& playerId) const noexcept
    {
        if (!this->_playerDatabaseGetPlayerIdByUserName->SetParameterString(0, userName))
        {
            shared::api::logging::Log("Failed to set username.");
            return false;
        }

        if (!this->_playerDatabaseGetPlayerIdByUserName->Run())
        {
            shared::api::logging::Log("Failed to run player database get player id by username statement.");
            return false;
        }

        auto results = this->_playerDatabaseGetPlayerIdByUserName->GetIntReturnValues();
        if (results.size() > 1)
        {
            shared::api::logging::Log("Incorrect number of results returned for get player id by username statement: " +
                             std::to_string(results.size()));
            return false;
        }

        playerId = results.empty() ? 0 : results[0];

        return true;
    }

    bool DataManager::GetCurrentWorldByPlayerId(uint32_t playerId, std::string& currentWorld) const noexcept
    {
        if (!this->_playerDatabaseGetCurrentWorldByPlayerId->SetParameterInt(0, playerId))
        {
            shared::api::logging::Log("Failed to set player id.");
            return false;
        }

        if (!this->_playerDatabaseGetCurrentWorldByPlayerId->Run())
        {
            shared::api::logging::Log("Failed to run player database get current world by player id statement.");
            return false;
        }

        auto results = this->_playerDatabaseGetCurrentWorldByPlayerId->GetStringReturnValues();
        if (results.size() > 1)
        {
            shared::api::logging::Log("Incorrect number of results returned for get current world by player id statement: " +
                             std::to_string(results.size()));
            return false;
        }

        currentWorld = results.empty() ? "" : results[0];

        return true;
    }

    bool DataManager::GetPosByPlayerId(uint32_t playerId, uint32_t& xPos, uint32_t& yPos) const noexcept
    {
        if (!this->_playerDatabaseGetPosByPlayerId->SetParameterInt(0, playerId))
        {
            shared::api::logging::Log("Failed to set player id.");
            return false;
        }

        if (!this->_playerDatabaseGetPosByPlayerId->Run())
        {
            shared::api::logging::Log("Failed to run player database get pos by player id statement.");
            return false;
        }

        auto results = this->_playerDatabaseGetPosByPlayerId->GetIntReturnValues();
        if (results.size() != 2)
        {
            shared::api::logging::Log("Incorrect number of results returned for get pos by player id statement: " +
                             std::to_string(results.size()));
            return false;
        }

        xPos = results[0];
        yPos = results[1];

        return true;
    }
}
