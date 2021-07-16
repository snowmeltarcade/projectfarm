#include <fstream>
#include <random>
#include <cmath>

#include "world.h"
#include "engine/entities/world_entity.h"
#include "utils/util.h"
#include "networking/packet_factory.h"
#include "networking/packets/server_client_load_world.h"
#include "networking/packets/server_client_entity_update.h"
#include "networking/packets/client_server_entity_update.h"
#include "networking/packets/server_client_player_joined_world.h"
#include "networking/packets/server_client_player_left_world.h"
#include "networking/packets/server_client_remove_entity_from_world.h"
#include "networking/packets/server_client_character_set_details.h"
#include "networking/packets/client_server_chatbox_message.h"
#include "networking/packets/server_client_chatbox_message.h"
#include "action_tile_actions/warp.h"
#include "server/server.h"
#include "time/clock.h"
#include "api/logging/logging.h"

using namespace std::literals;

namespace projectfarm::engine::world
{
    bool World::Load(const std::string& name, const std::filesystem::path& filePath)
    {
        this->_timer = std::make_shared<shared::time::Timer>();

        this->_plots->SetDataProvider(this->_dataProvider);
        if (!this->_plots->Load(name))
        {
            shared::api::logging::Log("Failed to load plots.");
            return false;
        }

        if (!this->LoadFromFile(filePath))
        {
            shared::api::logging::Log("Failed to load world from file: " + filePath.u8string());
            return false;
        }

        if (!this->AddWorldEntity())
        {
            shared::api::logging::Log("Failed to add world entity for world: " + this->_name);
            return false;
        }

        shared::api::logging::Log("Loaded world file: " + this->_name);

        return true;
    }

    bool World::LoadFromFile(const std::filesystem::path& filePath) noexcept
    {
        if (filePath.extension() == ".bin")
        {
            if (!this->LoadFromBinaryFile(filePath))
            {
                shared::api::logging::Log("Failed to load world from binary file: " + filePath.u8string());
                return false;
            }
        }
        else
        {
            if (!this->LoadFromJsonFile(filePath))
            {
                shared::api::logging::Log("Failed to load world from json file: " + filePath.u8string());
                return false;
            }
        }

        if (!this->InitiateScript())
        {
            shared::api::logging::Log("Failed to initiate script");
            return false;
        }

        return true;
    }

    bool World::LoadFromBinaryFile(const std::filesystem::path& filePath)
    {
        std::ifstream fs(filePath, std::ios::binary);

        if (!fs.is_open())
        {
            shared::api::logging::Log("Failed to open world file: " + filePath.u8string());
            return false;
        }

        this->_name = pfu::ReadStringFromBinaryFile(fs);

        if (!this->LoadIsland(fs))
        {
            shared::api::logging::Log("Failed to load island.");
            return false;
        }

        return true;
    }

    bool World::LoadFromJsonFile(const std::filesystem::path& filePath)
    {
        // nlohmann::json can throw exceptions
        try
        {
            std::ifstream file(filePath);

            if (!file.is_open())
            {
                shared::api::logging::Log("Failed to open world file: " + filePath.u8string());
                return false;
            }

            nlohmann::json jsonFile;
            file >> jsonFile;

            this->_name = jsonFile["name"].get<std::string>();

            const auto& islandsJson = jsonFile["islands"];

            for (const auto& islandJson : islandsJson)
            {
                if (!this->LoadIsland(islandJson))
                {
                    shared::api::logging::Log("Failed to load island: " + islandJson.dump());
                    return false;
                }
            }
        }
        catch (const std::exception& ex)
        {
            shared::api::logging::Log("Failed to read world file: " + filePath.u8string() +
                             "with exception: " + ex.what());

            return false;
        }

        return true;
    }

    bool World::LoadIsland(const nlohmann::json& islandJson)
    {
        auto island = std::make_shared<Island>();
        if (!island->LoadFromJson(islandJson, this->_plots))
        {
            shared::api::logging::Log("Failed to load island with json: " + islandJson.dump());
            return false;
        }

        this->_islands.emplace_back(std::move(island));

        return true;
    }

    bool World::LoadIsland(std::ifstream& fs) noexcept
    {
        auto island = std::make_shared<Island>();
        if (!island->LoadFromBinary(fs, this->_plots))
        {
            shared::api::logging::Log("Failed to load island with from binary file.");
            return false;
        }

        this->_islands.emplace_back(std::move(island));

        return true;
    }

    bool World::InitiateScript() noexcept
    {
        std::filesystem::path scriptName = "{ServerScriptsWorlds}";
        scriptName /= this->_name + ".js";
        auto scriptPath = this->_dataProvider->NormalizePath(scriptName);

        this->_script = this->_scriptSystem->CreateScript(shared::scripting::ScriptTypes::World, scriptPath);
        if (!this->_script)
        {
            shared::api::logging::Log("Failed to load world script: " + scriptName.u8string());
            return false;
        }

        this->_script->SetObjectInternalField(this);

        if (!this->_script->CallFunction(shared::scripting::FunctionTypes::Init, {}))
        {
            shared::api::logging::Log("Failed to call the 'init' function for script: " + scriptName.u8string());
            return false;
        }

        return true;
    }

    void World::Shutdown()
    {
        for (auto& entity : this->_entities)
        {
            entity->Deactivate();
        }
        this->_entities.clear();

        for (auto& island : this->_islands)
        {
            island->Shutdown();
        }
        this->_islands.clear();
    }

    bool World::AddWorldEntity() noexcept
    {
        auto worldEntity = this->CreateEntity<entities::WorldEntity>(0, this->shared_from_this());

        worldEntity->SetBroadcastStateTime(1000);

        this->_entities.emplace_back(std::move(worldEntity));

        return true;
    }

    bool World::RemoveWorldEntity(uint32_t entityId) noexcept
    {
        auto entityIt = std::find_if(this->_entities.begin(), this->_entities.end(), [=](const auto& e) { return e->GetEntityId() == entityId; });
        if (entityIt == this->_entities.end())
        {
            shared::api::logging::Log("Failed to find entity with id:" + std::to_string(entityId) + " to remove it.");
            return false;
        }

        return this->RemoveWorldEntity(*entityIt);
    }

    bool World::RemoveWorldEntity(const std::shared_ptr<engine::entities::Entity>& entity) noexcept
    {
        if (!entity)
        {
            shared::api::logging::Log("entity is null.");
            return false;
        }

        this->_entities.remove_if([&entity](const auto& e) { return e->GetEntityId() == entity->GetEntityId(); });

        entity->Deactivate();

        const auto serverClientRemoveEntityFromWorld = std::static_pointer_cast<shared::networking::packets::ServerClientRemoveEntityFromWorld>(
            shared::networking::PacketFactory::CreatePacket(
                shared::networking::PacketTypes::ServerClientRemoveEntityFromWorld));

        serverClientRemoveEntityFromWorld->SetEntityId(entity->GetEntityId());
        serverClientRemoveEntityFromWorld->SetWorldName(this->_name);

        this->SendPacketToAllPlayers(serverClientRemoveEntityFromWorld);

        const auto serverClientRemoveEntityFromWorld2 = std::static_pointer_cast<shared::networking::packets::ServerClientRemoveEntityFromWorld>(
                shared::networking::PacketFactory::CreatePacket(
                        shared::networking::PacketTypes::ServerClientRemoveEntityFromWorld));

        serverClientRemoveEntityFromWorld2->SetEntityId(entity->GetEntityId());
        serverClientRemoveEntityFromWorld2->SetWorldName(this->_name);

        // send a 2nd time just in case an entity update packet was received after
        // this packet was, thus re-creating the just removed entity
        this->SendPacketToAllPlayers(serverClientRemoveEntityFromWorld2, 0, 100);

        return true;
    }

    template <typename T, typename... Args>
    [[nodiscard]] std::shared_ptr<T> World::CreateEntity(uint32_t entityId, Args... args) const noexcept
    {
        auto entity = std::make_shared<T>(args...);
        entity->SetTimer(this->_timer);

        if (entityId > 0)
        {
            entity->SetEntityId(entityId);
        }

        return entity;
    }

    void World::Tick()
    {
        this->UpdateEntities();

        this->ProcessActionTileActions();

        this->_timer->IncrementFrame();
    }

    void World::UpdateEntities()
    {
        for (auto& entity : this->_entities)
        {
            entity->Tick();

            if (entity->ShouldBroadcastState())
            {
                this->BroadcastEntityState(entity, this->_timer->GetTotalGameDurationInMicroseconds());
            }
        }
    }

    std::vector<std::byte> World::GetDataForClientSerialization() const noexcept
    {
        std::vector<std::byte> data;

        pfu::WriteUInt32(data, static_cast<uint32_t>(this->_islands.size()));
        for (const auto& island : this->_islands)
        {
            auto changeLog = island->GetChangeLog();

            pfu::WriteUInt32(data, static_cast<uint32_t>(changeLog.size()));

            for (auto log = changeLog.rbegin(); log != changeLog.rend(); ++log)
            {
                pfu::WriteUInt8(data, log->LayerIndex);
                pfu::WriteUInt32(data, log->TileX);
                pfu::WriteUInt32(data, log->TileY);
                pfu::WriteUInt64(data, log->Time);
                pfu::WriteUInt16(data, log->PlotIndex);
            }
        }

        return data;
    }

    bool World::AddPlayer(const std::shared_ptr<engine::Player>& player,
                          uint32_t entityId, bool shouldUseSpawnPoints, bool isNewPlayerToGame) noexcept
    {
        player->SetCurrentWorld(this->shared_from_this());

        if (!this->_dataManager->UpdatePlayerCurrentWorld(player->GetPlayerId(), this->GetName()))
        {
            shared::api::logging::Log("Failed to update player current world. Player Id: " + std::to_string(player->GetPlayerId()) +
                             " World Name: " + this->GetName());
            return false;
        }

        std::shared_ptr<entities::Character> character;

        if (shouldUseSpawnPoints || isNewPlayerToGame)
        {
            // TODO: Add a 'startingspawnpoint'. This should be used when a player joins for the very first time
            auto tiles = this->GetActionTilesByProperty("type", "playerspawn");

            if (tiles.empty())
            {
                shared::api::logging::Log("Failed to find action tile: type=playerspawn");
                return false;
            }

            // we'll just choose the first one for now
            auto [islandId, actionTiles] = *tiles.begin();
            auto actionTile = actionTiles.begin();

            character = this->AddCharacter(actionTile->X, actionTile->Y, islandId,
                                           player->GetCharacterType(), entityId,
                                           player->GetPlayerId());
        }
        else
        {
            auto xPos {0u};
            auto yPos {0u};

            if (!this->_dataManager->GetPosByPlayerId(player->GetPlayerId(), xPos, yPos))
            {
                shared::api::logging::Log("Failed to get player pos with player id: " + std::to_string(player->GetPlayerId()));
                return false;
            }

            // TODO: Use the DRY method mentioned in Character::PersistPlayerState
            xPos = static_cast<uint32_t>(static_cast<float>(xPos) * 0.0001f);
            yPos = static_cast<uint32_t>(static_cast<float>(yPos) * 0.0001f);

            character = this->AddCharacter(xPos, yPos, player->GetCharacterType(),
                                           entityId, player->GetPlayerId());
        }

        if (!character)
        {
            shared::api::logging::Log("Failed to create character: " + player->GetCharacterType());
            return false;
        }

        player->SetCharacter(character);

        this->_players.push_back(player->GetPlayerId());

        // we need to start the world load on the client before they receive the
        // character details packet
        const auto serverClientLoadWorldPacket = std::static_pointer_cast<shared::networking::packets::ServerClientLoadWorldPacket>(
                shared::networking::PacketFactory::CreatePacket(
                        shared::networking::PacketTypes::ServerClientLoadWorld));

        serverClientLoadWorldPacket->SetWorldToLoad(this->GetName());
        this->_packetSender->AddPacketToSend(player->GetNetworkClient()->GetSocket(), serverClientLoadWorldPacket);

        return true;
    }

    void World::RemovePlayer(uint32_t playerId) noexcept
    {
        const auto serverClientPlayerLeftWorld = std::static_pointer_cast<shared::networking::packets::ServerClientPlayerLeftWorld>(
            shared::networking::PacketFactory::CreatePacket(
                shared::networking::PacketTypes::ServerClientPlayerLeftWorld));

        serverClientPlayerLeftWorld->SetPlayerId(playerId);
        serverClientPlayerLeftWorld->SetWorldName(this->_name);

        this->SendPacketToAllPlayers(serverClientPlayerLeftWorld, playerId);

        auto player = this->GetServer()->GetPlayerById(playerId);
        if (!player)
        {
            shared::api::logging::Log("Failed to get player with id: " + std::to_string(playerId));
            return;
        }

        this->BroadcastChatboxSystemMessage("Player `" + player->GetUsername() + "` has left this world.",
                                            playerId);

        this->_players.remove(playerId);

        if (!this->RemoveWorldEntity(player->GetCharacter()))
        {
            shared::api::logging::Log("Failed to remove player's character with player id: " + std::to_string(playerId));
        }

        player->SetCurrentWorld(nullptr);
        player->SetCharacter(nullptr);
    }

    bool World::TransferPlayerToWorld(uint32_t playerId, uint32_t entityId,
                                      const std::string& destinationWorldName) noexcept
    {
        this->RemovePlayer(playerId);

        if (!this->GetServer()->AddPlayerToWorld(playerId, entityId, destinationWorldName))
        {
            shared::api::logging::Log("Failed to move player with id: " + std::to_string(playerId) +
                             " to world: " + destinationWorldName +
                             " with entity id: " + std::to_string(entityId));
            return false;
        }

        return true;
    }

    bool World::TransferCharacterToWorld(const std::shared_ptr<entities::Character>& character,
                                         const std::string& destinationWorldName,
                                         const std::string& destinationTileType) noexcept
    {
        if (!this->_dataManager->UpdateEntityAppearance(character->GetEntityId(), character->GetAppearanceDetails()))
        {
            shared::api::logging::Log("Failed to update entity appearance for entity with id: "s +
                std::to_string(character->GetEntityId()));
            return false;
        }

        if (!this->RemoveWorldEntity(character))
        {
            shared::api::logging::Log("Failed to remove entity with entity id: " + std::to_string(character->GetEntityId()) +
                             " from world: " + this->_name);
            return false;
        }

        if (!this->GetServer()->AddCharacterToWorld(character->GetCharacterType(),
                                                    destinationWorldName, destinationTileType,
                                                    character->GetEntityId(),
                                                    character->GetPlayerId()))
        {
            shared::api::logging::Log("Failed to move entity with entity id: " + std::to_string(character->GetEntityId()) +
                             " to world: " + destinationWorldName);
            return false;
        }

        return true;
    }

    void World::OnReceivePacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                const std::shared_ptr<engine::Player>& player) noexcept
    {
        if (packet->GetPacketType() == shared::networking::PacketTypes::ClientServerWorldLoaded)
        {
            this->HandleClientServerWorldLoadedPacket(packet, player);
        }
        else if (packet->GetPacketType() == shared::networking::PacketTypes::ClientServerEntityUpdate)
        {
            this->HandleClientServerEntityUpdatePacket(packet, player);
        }
        else if (packet->GetPacketType() == shared::networking::PacketTypes::ClientServerChatboxMessage)
        {
            this->HandleClientServerChatboxMessagePacket(packet, player);
        }
        else
        {
            auto packetType = static_cast<uint32_t>(packet->GetPacketType());
            shared::api::logging::Log("Unknown packet type: " + std::to_string(packetType));
        }
    }

    std::vector<std::shared_ptr<entities::Character>> World::GetCharactersWithinDistance(
            uint32_t entityId, float distance) const noexcept
    {
        std::vector<std::shared_ptr<entities::Character>> characters;

        // TODO: Optimize this...
        for (const auto& entity : this->_entities)
        {
            auto c = std::dynamic_pointer_cast<entities::Character>(entity);
            if (!c)
            {
                continue;
            }

            characters.push_back(c);
        }

        auto fromCharacter = std::find_if(characters.begin(), characters.end(),
                                          [entityId](const auto& c) { return c->GetEntityId() == entityId; });

        if (fromCharacter == characters.end())
        {
            return {};
        }

        std::vector<std::shared_ptr<entities::Character>> charactersInRange;

        auto [x, y] = (*fromCharacter)->GetLocation();

        for (const auto& c : characters)
        {
            if (c->GetEntityId() == entityId)
            {
                continue;
            }

            auto [cx, cy] = c->GetLocation();

            auto dx = std::abs(x - cx);
            auto dy = std::abs(y - cy);

            auto cDistance = static_cast<float>(std::sqrt((dx*dx) + (dy*dy)));

            if (cDistance <= distance)
            {
                charactersInRange.push_back(c);
            }
        }

        return charactersInRange;
    }

    void World::BroadcastEntityState(const std::shared_ptr<engine::entities::Entity>& entity,
            uint64_t currentTime) const noexcept
    {
        const auto serverClientEntityUpdatePacket = std::static_pointer_cast<shared::networking::packets::ServerClientEntityUpdatePacket>(
                shared::networking::PacketFactory::CreatePacket(
                        shared::networking::PacketTypes::ServerClientEntityUpdate));

        serverClientEntityUpdatePacket->SetEntityId(entity->GetEntityId());
        serverClientEntityUpdatePacket->SetPlayerId(entity->GetPlayerId());
        serverClientEntityUpdatePacket->SetWorldName(this->_name);
        serverClientEntityUpdatePacket->SetTimeOfUpdate(currentTime);
        serverClientEntityUpdatePacket->SetEntityType(entity->GetEntityType());

        auto data = entity->GetEntityData();

        serverClientEntityUpdatePacket->SetEntityData(data);

        auto exceptPlayerId {0u};

        if (!entity->GetForceSendToOwningPlayer())
        {
            if (auto character = std::dynamic_pointer_cast<engine::entities::Character>(entity);
                character)
            {
                exceptPlayerId = character->GetPlayerId();
            }
        }

        // Typically we are sending vital data when forcing it to go to the
        // owning player. If this is the case, mark the packet as vital.
        this->SendPacketToAllPlayers(serverClientEntityUpdatePacket, exceptPlayerId,
                                     0u, entity->GetForceSendToOwningPlayer());

        entity->ResetBroadcastCounter();
        entity->SetForceSendToOwningPlayer(false);

        entity->OnAfterBroadcastState();
    }

    void World::SendPacketToAllPlayers(const std::shared_ptr<shared::networking::Packet>& packet,
            uint32_t exceptPlayerId, uint64_t milliseconds, bool forcePacketVital) const noexcept
    {
        for (const auto& playerId : this->_players)
        {
            if (playerId == exceptPlayerId)
            {
                continue;
            }

            auto player = this->GetServer()->GetPlayerById(playerId);
            if (!player)
            {
                shared::api::logging::Log("Failed to get player with id: " + std::to_string(playerId));
                continue;
            }

            // check that the player has not just left this world
            auto playerWorld = player->GetCurrentWorld();
            if (!playerWorld || playerWorld->GetName() != this->_name)
            {
                continue;
            }

            if (packet->IsVital() || forcePacketVital)
            {
                this->_packetSender->AddPacketToSend(player->GetNetworkClient()->GetSocket(), packet,
                                                     milliseconds);
            }
            else
            {
                this->_packetSender->AddPacketToSend(player->GetUDPIPAddress(), packet,
                                                     milliseconds);
            }
        }
    }

    std::shared_ptr<entities::Character> World::AddCharacter(uint32_t tileX, uint32_t tileY, uint8_t islandId,
                                                             const std::string& type, uint32_t entityId,
                                                             uint32_t playerId) noexcept
    {
        auto [xPos, yPos] = this->_islands[islandId]->GetWorldPositionFromTileCoordinate(tileX, tileY);

        auto character = this->AddCharacter(static_cast<uint32_t>(xPos), static_cast<uint32_t>(yPos),
                                            type, entityId, playerId);
        return character;
    }

    std::shared_ptr<entities::Character> World::AddCharacter(uint32_t xPos, uint32_t yPos, const std::string& type,
                                                             uint32_t entityId, uint32_t playerId) noexcept
    {
        auto character = this->CreateCharacter(type, entityId, playerId);
        if (!character)
        {
            shared::api::logging::Log("Failed to create character of type: " + type);
            return nullptr;
        }

        character->SetLocation(static_cast<float>(xPos), static_cast<float>(yPos));
        character->SetPlayerId(playerId);
        character->SetBroadcastStateTime(50); // TODO: This value need to be set somehow...

        // copy the pointer here
        this->_entities.push_back(character);

        character->Activate();

        this->SendSetCharacterDetailsPacket(character);

        return character;
    }

    std::shared_ptr<entities::Character> World::AddCharacter(const std::string& characterType,
                                                             const std::string& destinationTileType,
                                                             uint32_t entityId, uint32_t playerId) noexcept
    {
        auto actionTiles = this->GetActionTilesByProperty("type", destinationTileType);
        if (actionTiles.empty())
        {
            shared::api::logging::Log("Failed to find warp destination tile type for world: " + this->_name);
            return nullptr;
        }

        std::shared_ptr<entities::Character> character;

        // TODO: For now just get the first tile
        for (const auto& [islandIndex, tiles] : actionTiles)
        {
            if (tiles.empty())
            {
                shared::api::logging::Log("Failed to find warp tiles for world: " + this->_name);
                return nullptr;
            }

            auto tile = tiles[0];

            character = this->AddCharacter(tile.X, tile.Y, islandIndex, characterType, entityId, playerId);
            // TODO: For now just get the first tile
            if (character)
            {
                break;
            }
        }

        return character;
    }

    std::shared_ptr<entities::Character> World::CreateCharacter(const std::string& type,
                                                                uint32_t entityId, uint32_t playerId) noexcept
    {
        auto character = this->CreateEntity<entities::Character>(entityId);
        character->SetDataProvider(this->_dataProvider);
        character->SetScriptSystem(this->_scriptSystem);
        character->SetDataManager(this->_dataManager);
        character->SetPlayerId(playerId);

        auto filePath = this->_dataProvider->GetCharacterPathFromName(type);
        if (filePath.empty())
        {
            shared::api::logging::Log("Failed to get file path for character type: " + type);
            return nullptr;
        }

        if (!character->LoadFromFile(filePath))
        {
            shared::api::logging::Log("Failed to load character from file: " + filePath.u8string());
            return nullptr;
        }

        character->SetCurrentWorld(this->shared_from_this());

        auto isNewEntity = entityId == 0;
        shared::entities::CharacterAppearanceDetails appearanceDetails;

        if (playerId > 0)
        {
            if (this->_dataManager->GetPlayerAppearance(playerId, appearanceDetails))
            {
                // Likely a new character, so use the default if we cannot find
                // their details
                character->SetAppearanceDetails(appearanceDetails);
            }
        }
        else if (!isNewEntity)
        {
            if (!this->_dataManager->GetEntityAppearance(entityId, appearanceDetails))
            {
                shared::api::logging::Log("Failed to get entity appearance for entity id: " + std::to_string(entityId));
                return nullptr;
            }

            character->SetAppearanceDetails(appearanceDetails);
        }

        if (!character->ConfirmAppearance(this->_actionAnimationsManager))
        {
            shared::api::logging::Log("Failed to confirm character appearance with entity id: " + std::to_string(character->GetEntityId()));
            return nullptr;
        }

        if (!this->_dataManager->UpdateEntityAppearance(character->GetEntityId(), character->GetAppearanceDetails(),
                                                        isNewEntity))
        {
            shared::api::logging::Log("Failed to insert entity appearance with entity id: " +
                             std::to_string(character->GetEntityId()));
            return nullptr;
        }

        return character;
    }

    void World::HandleClientServerWorldLoadedPacket(const std::shared_ptr<shared::networking::Packet>&,
                                                    const std::shared_ptr<engine::Player>& player) noexcept
    {
        shared::api::logging::Log("Player: " + player->GetUsername() + " has loaded world: " + this->_name);

        const auto serverClientPlayerJoinedWorld = std::static_pointer_cast<shared::networking::packets::ServerClientPlayerJoinedWorld>(
                shared::networking::PacketFactory::CreatePacket(
                        shared::networking::PacketTypes::ServerClientPlayerJoinedWorld));

        serverClientPlayerJoinedWorld->SetPlayerId(player->GetPlayerId());
        serverClientPlayerJoinedWorld->SetWorldName(this->_name);

        this->SendPacketToAllPlayers(serverClientPlayerJoinedWorld);

        auto character = player->GetCharacter();

        // perhaps the world on the client took a while to load
        // ensure that they get a creation message to create the player
        character->SetForceSendToOwningPlayer(true);

        // ensure that the player starts as the specific position, and
        // doesn't lerp from some other position
        character->SetLerpPositionChangeOnClient(false);

        for (const auto& entity : this->_entities)
        {
            this->SendSetCharacterDetailsPacket(entity);
        }

        this->BroadcastChatboxSystemMessage("Player `" + player->GetUsername() + "` has joined this world.",
                                            player->GetPlayerId());
    }

    void World::HandleClientServerEntityUpdatePacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                                     const std::shared_ptr<engine::Player>& player) noexcept
    {
        auto clientServerEntityUpdatePacket =
                std::static_pointer_cast<shared::networking::packets::ClientServerEntityUpdatePacket>(packet);

        auto character = player->GetCharacter();

        if (!character)
        {
            shared::api::logging::Log("Failed to get character for player id: " + std::to_string(player->GetPlayerId()));
            return;
        }

        if (clientServerEntityUpdatePacket->GetEntityId() != character->GetEntityId())
        {
            shared::api::logging::Log("Client trying to update a packet that isn't the clients players. Player Id: "
                + std::to_string(player->GetPlayerId()) + ". Entity Id: "
                + std::to_string(clientServerEntityUpdatePacket->GetEntityId()));
            return;
        }

        // as the times in this packet are from the player, we will work with those times
        // and not the time on the server
        auto lastUpdateTime = clientServerEntityUpdatePacket->GetTimeOfUpdate();
        if (lastUpdateTime <= character->GetLastUpdateTime())
        {
            return;
        }

        character->SetData(clientServerEntityUpdatePacket->GetEntityData());

        character->SetLastUpdateTime(lastUpdateTime);
    }

    void World::HandleClientServerChatboxMessagePacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                                       const std::shared_ptr<engine::Player>& player) noexcept
    {
        auto clientServerChatboxMessagePacket =
                std::static_pointer_cast<shared::networking::packets::ClientServerChatboxMessagePacket>(packet);

        auto message = clientServerChatboxMessagePacket->GetMessage();

        this->BroadcastChatboxMessage(message, player->GetUsername(), player->GetPlayerId());
    }

    uint16_t World::GetPlotIndexFromWorldPosition(float x, float y) const noexcept
    {
        for (const auto& island : this->_islands)
        {
            auto index = island->GetPlotIndexAtWorldPosition(x, y);
            if (index != Plots::EmptyIndex)
            {
                return index;
            }
        }

        return Plots::EmptyIndex;
    }

    std::tuple<uint8_t, int32_t, int32_t>
        World::GetTileIndexesFromWorldPosition(float x, float y) const noexcept
    {
        uint8_t islandIndex = 0u;
        for (const auto& island : this->_islands)
        {
            auto [tx, ty] = island->GetTileIndexesFromWorldPosition(x, y);
            if (tx != -1 && ty != -1)
            {
                return {islandIndex, tx, ty};
            }

            ++islandIndex;
        }

        return {0u, -1, -1};
    }

    std::map<uint8_t, std::vector<ActionTile>>
        World::GetActionTilesByProperty(const std::string& name, const std::string& value) const noexcept
    {
        std::map<uint8_t, std::vector<ActionTile>> tilesMap;

        uint8_t index = 0u;
        for (const auto& island : this->_islands)
        {
            std::vector<ActionTile> tiles;

            auto islandTiles = island->GetActionTilesByProperty(name, value);
            tiles.insert(tiles.end(), islandTiles.begin(), islandTiles.end());

            tilesMap[index++] = std::move(tiles);
        }

        return tilesMap;
    }

    void World::TriggerActionTiles(float worldX, float worldY,
            const std::shared_ptr<entities::Character>& character) noexcept
    {
        auto [islandIndex, tx, ty] = this->GetTileIndexesFromWorldPosition(worldX, worldY);

        if (tx == -1 || ty == -1)
        {
            return;
        }

        auto value = this->_islands[islandIndex]->GetPropertyValueForTile("action", tx, ty);
        if (value.empty())
        {
            return;
        }

        if (value == "warp")
        {
            auto destinationWorld = this->_islands[islandIndex]->GetPropertyValueForTile("warp_world", tx, ty);
            auto destinationTileType = this->_islands[islandIndex]->GetPropertyValueForTile("warp_type", tx, ty);

            if (destinationWorld.empty() || destinationTileType.empty())
            {
                shared::api::logging::Log("Failed to get properties for warp tile at: " + std::to_string(tx)
                    + ":" + std::to_string(ty));
                return;
            }

            auto warp = std::make_shared<action_tile_actions::Warp>(character, destinationWorld, destinationTileType);

            this->PushActionTileAction(std::static_pointer_cast<action_tile_actions::ActionTileActionBase>(warp));
        }
    }

    void World::ProcessActionTileActions() noexcept
    {
        for (const auto& action : this->_actionTileActions)
        {
            if (!action->Run())
            {
                shared::api::logging::Log("Failed to run action tile action.");
                continue;
            }
        }

        this->_actionTileActions.clear();
    }

    void World::SendSetCharacterDetailsPacket(const std::shared_ptr<entities::Entity>& entity) noexcept
    {
        auto character = std::dynamic_pointer_cast<entities::Character>(entity);
        if (!character)
        {
            return;
        }

        const auto serverClientSetCharacterDetails = std::static_pointer_cast<shared::networking::packets::ServerClientCharacterSetDetailsPacket>(
                shared::networking::PacketFactory::CreatePacket(
                        shared::networking::PacketTypes::ServerClientCharacterSetDetails));

        serverClientSetCharacterDetails->SetEntityId(character->GetEntityId());
        serverClientSetCharacterDetails->SetWorldName(this->_name);
        serverClientSetCharacterDetails->SetAppearanceDetails(character->GetAppearanceDetails());

        this->SendPacketToAllPlayers(serverClientSetCharacterDetails);
    }

    void World::BroadcastChatboxMessage(const std::string& message,
                                        const std::string& fromUsername,
                                        uint32_t exceptPlayerId) noexcept
    {
        const auto serverClientChatboxMessagePacket = std::static_pointer_cast<shared::networking::packets::ServerClientChatboxMessagePacket>(
                shared::networking::PacketFactory::CreatePacket(
                        shared::networking::PacketTypes::ServerClientChatboxMessage));

        serverClientChatboxMessagePacket->SetUsername(fromUsername);
        serverClientChatboxMessagePacket->SetMessage(message);
        serverClientChatboxMessagePacket->SetServerTime(shared::time::Clock::MillisecondsSinceEpoch());

        this->SendPacketToAllPlayers(serverClientChatboxMessagePacket, exceptPlayerId);
    }
}