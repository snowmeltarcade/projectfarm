#include <string>
#include <thread>
#include <SDL_net.h>

#include "server.h"
#include "networking/udp_packet_base.h"
#include "networking/packet_factory.h"
#include "networking/packets/server_client_set_player_details.h"
#include "networking/packets/client_server_player_authenticate.h"
#include "networking/packets/client_server_request_hashed_password.h"
#include "networking/packets/server_client_send_hashed_password.h"
#include "api/logging/logging.h"

namespace projectfarm::server
{
	void Server::Run(int argc, char* argv[])
	{
		shared::api::logging::Log("Starting server...");

        this->_systemArguments.SetArguments(argc, argv);

		if (!this->Initialize())
        {
		    shared::api::logging::Log("Failed to initialize server.");
		    return;
        }

		this->MainLoop();

		this->Shutdown();

		shared::api::logging::Log("Exiting server.");
	}

	bool Server::Initialize()
	{
		shared::api::logging::Log("Initializing server...");

		if (SDL_Init(SDL_INIT_EVENTS) < 0)
        {
		    shared::api::logging::Log("Failed to init SDL.");
            shared::api::logging::Log(SDL_GetError());
		    return false;
        }

		if (!this->_cryptoProvider->Initialize())
        {
		    shared::api::logging::Log("Failed to initialize crypto provider.");
		    return false;
        }

        this->_dataProvider = std::make_shared<projectfarm::shared::DataProvider>(this->_systemArguments.GetBinaryPath());
		if (!this->_dataProvider->SetupServer())
        {
		    shared::api::logging::Log("Failed to setup data provider.");
		    return false;
        }

		this->_serverConfig->SetDataProvider(this->_dataProvider);
		if (!this->_serverConfig->LoadConfig())
        {
		    shared::api::logging::Log("Failed to load server config.");
		    return false;
        }

		this->_randomEngine->Initialize();

		this->_dataManager->SetDataProvider(this->_dataProvider);
		if (!this->_dataManager->Initialize())
        {
		    shared::api::logging::Log("Failed to initialize data manager.");
		    return false;
        }

		if (!this->_networking.Initialize())
		{
			shared::api::logging::Log("Failed to initialize networking.");
			return false;
		}

		if (!this->_packetSender->Initialize())
		{
			shared::api::logging::Log("Failed to initialize packet sender.");
			return false;
		}

        if (!this->_clientConnectionManager.Initialize(this->_serverConfig))
        {
            shared::api::logging::Log("Failed to initialize client connection manager.");
            return false;
        }

        this->_scriptSystem->SetScriptFactory(this->_scriptFactory);
        this->_scriptSystem->SetDataProvider(this->_dataProvider);
        this->_scriptSystem->SetRandomEngine(this->_randomEngine);
        if (!this->_scriptSystem->Initialize(this->_systemArguments.GetBinaryPath()))
        {
            shared::api::logging::Log("Failed to initialize script system.");
            return false;
        }

        this->_actionAnimationsManager->SetDataProvider(this->_dataProvider);
        this->_actionAnimationsManager->SetRandomEngine(this->_randomEngine);
        if (!this->_actionAnimationsManager->Load())
        {
            shared::api::logging::Log("Failed to load the action animation manager.");
            return false;
        }

		if (!this->CreateWorlds())
        {
		    shared::api::logging::Log("Failed to create worlds.");
		    return false;
        }

		this->_shouldQuit = false;

		shared::api::logging::Log("Initialized server.");

		return true;
	}

	void Server::MainLoop()
	{
		using namespace std::chrono_literals;

        auto thisServer = this->shared_from_this();

		while (!this->_shouldQuit)
		{
		    this->HandleEvents();

            this->_clientConnectionManager.Tick(thisServer);

            this->UpdateWorlds();
		}
	}

	void Server::HandleEvents()
    {
	    SDL_Event event;
	    while (SDL_PollEvent(&event))
        {
	        switch (event.type)
            {
                case SDL_QUIT:
                {
                    this->TellServerToQuit();
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    void Server::UpdateWorlds()
    {
	    for (const auto& world : this->_worlds)
        {
            world->Tick();
        }
    }

	void Server::TellServerToQuit()
    {
        this->_shouldQuit = true;
    }

	void Server::Shutdown()
	{
		shared::api::logging::Log("Shutting down server...");

        for (const auto& world : this->_worlds)
        {
            world->Shutdown();
        }
        this->_worlds.clear();

        this->_players.clear();

        this->_scriptSystem->Shutdown();
		this->_packetSender->Shutdown();
        this->_clientConnectionManager.Shutdown();
		this->_networking.Shutdown();
		this->_dataManager->Shutdown();

		SDL_Quit();

		shared::api::logging::Log("Shut down server...");
	}

	bool Server::CreateWorld(const std::string& name, const std::filesystem::path& worldFilePath)
	{
		auto world = std::make_shared<engine::world::World>();
        world->SetDataProvider(this->_dataProvider);
        world->SetServer(this->GetPtr());
        world->SetPacketSender(this->_packetSender);
        world->SetScriptSystem(this->_scriptSystem);
        world->SetActionAnimationsManager(this->_actionAnimationsManager);
        world->SetDataManager(this->_dataManager);

		if (!world->Load(name, worldFilePath))
		{
			shared::api::logging::Log("Failed to load world: " + worldFilePath.u8string());
			return false;
		}

		this->_worlds.emplace_back(std::move(world));

		return true;
	}

	bool Server::CreateWorlds()
    {
	    auto locations = this->_dataProvider->GetWorldLocations();

        auto result = std::all_of(locations.begin(), locations.end(), [this](const auto& locationsItem)
        {
            auto& [name, location] = locationsItem;

            if (!this->CreateWorld(name, location))
            {
                shared::api::logging::Log("Failed to create session with location: " + location.u8string());
                return false;
            }

            return true;
        });

	    return result;
    }

    void Server::OnClientAdd(const std::shared_ptr<Client>& client) noexcept
    {
        auto player = std::make_shared<engine::Player>(client);
        this->_unauthenticatedPlayers.emplace_back(std::move(player));
    }

    void Server::OnClientRemove(const std::shared_ptr<Client>& client) noexcept
    {
        auto playerIt = std::find_if(this->_players.begin(), this->_players.end(),
                                     [&client](const auto& player)
                                     {
                                         return player->GetNetworkClient() == client;
                                     });

        if (playerIt == this->_players.end())
        {
            shared::api::logging::Log("Failed to find player with IP: " + client->IPAddressAsString());
            return;
        }

        auto world = (*playerIt)->GetCurrentWorld();

        if (world)
        {
            world->RemovePlayer((*playerIt)->GetPlayerId());
        }

        this->_players.erase(playerIt);

        shared::api::logging::Log("Removed player with IP: " + client->IPAddressAsString());
    }

    void Server::OnReceivePacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                 const std::shared_ptr<Client>& client,
                                 const IPaddress& ipAddress) noexcept
    {
	    auto packetType = packet->GetPacketType();

	    auto player = this->FindPlayer(packet, client);
	    if (player == nullptr)
        {
	        shared::api::logging::Log("Failed to find player, ignoring packet.");
	        return;
        }

        if (packetType == shared::networking::PacketTypes::ClientServerPlayerAuthenticate)
        {
            this->HandleClientServerPlayerAuthenticatePacket(packet, player);
        }
        else if (packetType == shared::networking::PacketTypes::ClientServerRequestHashedPassword)
        {
            this->HandleClientServerRequestHashedPasswordPacket(packet, player);
        }
        else if (packetType == shared::networking::PacketTypes::ClientServerTestUdp)
        {
            this->HandleClientServerTestUdpPacket(packet, ipAddress, player);
        }
	    else
        {
            auto world = player->GetCurrentWorld();
            if (world)
            {
                world->OnReceivePacket(packet, player);
            }
            else
            {
                shared::api::logging::Log("Player has no world, ignoring packet.");
                return;
            }
        }
    }

    bool Server::AddPlayerToWorld(uint32_t playerId, uint32_t entityId,
                                  const std::string& destinationWorldName) noexcept
    {
	    auto playerIter = std::find_if(this->_players.begin(), this->_players.end(),
                                       [playerId](const auto& player) { return player->GetPlayerId() == playerId; });

	    if (playerIter == this->_players.end())
        {
	        shared::api::logging::Log("Failed to find player with id: " + std::to_string(playerId));
	        return false;
        }

	    auto worldIter = std::find_if(this->_worlds.begin(), this->_worlds.end(),
	                                  [&destinationWorldName](const auto& world) { return world->GetName() == destinationWorldName; });

	    if (worldIter == this->_worlds.end())
        {
	        shared::api::logging::Log("Failed to find world with name: " + destinationWorldName);
	        return false;
        }

	    auto world = *worldIter;
	    auto player = *playerIter;

	    if (!world->AddPlayer(player, entityId))
        {
	        shared::api::logging::Log("Failed to add player with id: " + std::to_string(player->GetPlayerId()) +
                                      " to world: " + destinationWorldName);
	        return false;
        }

	    return true;
    }

    bool Server::AddCharacterToWorld(const std::string& characterType,
                                     const std::string& destinationWorldName,
                                     const std::string& destinationTileType,
                                     uint32_t entityId, uint32_t playerId) noexcept
    {
        auto worldIter = std::find_if(this->_worlds.begin(), this->_worlds.end(),
                                      [&destinationWorldName](const auto& world)
                                      {
                                        return world->GetName() == destinationWorldName;
                                      });

        if (worldIter == this->_worlds.end())
        {
            shared::api::logging::Log("Failed to find world with name: " + destinationWorldName);
            return false;
        }

        auto world = *worldIter;

        if (!world->AddCharacter(characterType, destinationTileType, entityId, playerId))
        {
            shared::api::logging::Log("Failed to add character: " + characterType +
                                      " to world: " + world->GetName() +
                                      " with entity id: " + std::to_string(entityId));
            return false;
        }

        return true;
    }

    std::shared_ptr<engine::Player> Server::GetPlayerById(uint32_t playerId) const noexcept
    {
        auto player = std::find_if(this->_players.begin(), this->_players.end(),
                                   [playerId](const auto& p) { return p->GetPlayerId() == playerId; });

        if (player == this->_players.end())
        {
            shared::api::logging::Log("Failed to find player with id: " + std::to_string(playerId));
            return nullptr;
        }

        return *player;
    }

    std::shared_ptr<engine::Player> Server::FindPlayer(
            const std::shared_ptr<shared::networking::Packet>& packet,
            const std::shared_ptr<Client>& client) const noexcept
    {
        if (auto player = this->FindPlayer(packet, client, this->_players); player)
        {
            return player;
        }

        if (auto player = this->FindPlayer(packet, client, this->_unauthenticatedPlayers); player)
        {
            return player;
        }

        return nullptr;
    }

    std::shared_ptr<engine::Player> Server::FindPlayer(
            const std::shared_ptr<shared::networking::Packet>& packet,
            const std::shared_ptr<Client>& client,
            const std::vector<std::shared_ptr<engine::Player>>& playersList) const noexcept
    {
	    // TCP packets use the network client to identity sender
	    if (packet->IsVital())
        {
            auto playerIt = std::find_if(playersList.begin(), playersList.end(),
                                      [&client](const auto& player)
                                        {
                                            return player->GetNetworkClient() == client;
                                        });

            if (playerIt == playersList.end())
            {
                shared::api::logging::Log("Failed to find player with IP: " + client->IPAddressAsString());
                return nullptr;
            }

            return *playerIt;
        }
	    // UDP packets send the player id
	    else
        {
	        auto udpPacket = std::static_pointer_cast<shared::networking::UDPPacketBase>(packet);
	        if (!udpPacket)
            {
	            shared::api::logging::Log("Failed to get UDP packet base.");
	            return nullptr;
            }

	        auto playerId = udpPacket->GetPlayerId();

            auto playerIt = std::find_if(playersList.begin(), playersList.end(),
                                         [&playerId](const auto& player)
                                         {
                                             return player->GetPlayerId() == playerId;
                                         });

            if (playerIt == playersList.end())
            {
                shared::api::logging::Log("Failed to find player with player id: " + std::to_string(playerId));
                return nullptr;
            }

            return *playerIt;
        }
    }

    void Server::HandleClientServerTestUdpPacket(const std::shared_ptr<shared::networking::Packet>&,
                                                 const IPaddress& ipAddress,
                                                 const std::shared_ptr<engine::Player>& player) noexcept
    {
        // this packet will be spammed to the server, so there isn't a need to handle this more than once
        if (player->GetUDPIPAddress() == ipAddress)
        {
            return;
        }

        player->SetUDPIPAddress(ipAddress);

        std::string currentWorld;
        if (!this->_dataManager->GetCurrentWorldByPlayerId(player->GetPlayerId(), currentWorld))
        {
            shared::api::logging::Log("Failed to get current world for player with id: " + std::to_string(player->GetPlayerId()));
            return;
        }

        auto isNewPlayerToGame = currentWorld.empty();

        if (isNewPlayerToGame)
        {
            currentWorld = this->_serverConfig->GetStartingWorld();
        }

        auto world = std::find_if(this->_worlds.begin(), this->_worlds.end(),
                                  [&currentWorld](const auto& w)
                                  {
                                      return w->GetName() == currentWorld;
                                  });

        if (world == this->_worlds.end())
        {
            shared::api::logging::Log("Failed to find world with name: " + currentWorld);
            return;
        }

        if (!(*world)->AddPlayer(player, 0, false, isNewPlayerToGame))
        {
            // TODO: Inform the player of the error...
            shared::api::logging::Log("Failed to add player for ip address: " + std::to_string(ipAddress.host));
            return;
        }
    }

    std::shared_ptr<engine::PlayerLoadDetails> Server::GetPlayerLoadDetails(uint32_t playerId) const noexcept
    {
	    auto loadDetails = std::make_shared<engine::PlayerLoadDetails>();

        if (!this->_dataManager->GetPlayerLoadDetails(playerId, loadDetails->CharacterType))
        {
            shared::api::logging::Log("Failed to get character load details with player id: " +
                                      std::to_string(playerId));
            return nullptr;
        }

	    return loadDetails;
    }

    void Server::InitializePlayer(std::shared_ptr<engine::Player>& player,
                                  const std::shared_ptr<engine::PlayerLoadDetails>& loadDetails) noexcept
    {
        player->SetLoadDetails(loadDetails);
    }

    void Server::HandleClientServerPlayerAuthenticatePacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                                            std::shared_ptr<engine::Player>& player) noexcept
    {
        auto clientServerPlayerAuthenticatePacket = std::static_pointer_cast<
                shared::networking::packets::ClientServerPlayerAuthenticatePacket>(packet);

        auto userName = clientServerPlayerAuthenticatePacket->GetUserName();
        auto hashedPassword = clientServerPlayerAuthenticatePacket->GetHashedPassword();

        uint32_t playerId {0};
        if (!this->_dataManager->GetPlayerIdByUserName(userName, playerId))
        {
            shared::api::logging::Log("Failed to get player id by username: " + userName);
            return;
        }

        if (playerId == 0)
        {
            if (!this->_dataManager->InsertPlayer(userName, hashedPassword, playerId))
            {
                shared::api::logging::Log("Failed to insert player with username: " + userName);
                return;
            }

            shared::api::logging::Log("Successfully inserted player with username: " + userName +
                                      ". Has player id: " + std::to_string(playerId));
        }
        else
        {
            if (!this->_dataManager->UpdatePlayerLogin(userName, playerId))
            {
                shared::api::logging::Log("Failed to update player login with username: " + userName);
                return;
            }

            shared::api::logging::Log("Successfully updated player login with username: " + userName +
                                      ". Has player id: " + std::to_string(playerId));
        }

        // TODO: If the player was newly inserted, send to load the player's appearance
        // or have the player's appearance as part of the registration screen
        // For now, the character_type is set in the .sql file and the appearance
        // details will just use the default of the character

	    auto loadDetails = this->GetPlayerLoadDetails(playerId);
        if (!loadDetails)
        {
            shared::api::logging::Log("Failed to get player load details with player id: " + std::to_string(playerId));
            return;
        }

	    this->InitializePlayer(player, loadDetails);

        player->SetPlayerId(playerId);
        player->SetUsername(userName);

        // server (player details) -> client (udp test) -> server (load world)
        const auto serverClientSetPlayerDetails = std::static_pointer_cast<shared::networking::packets::ServerClientSetPlayerDetails>(
                shared::networking::PacketFactory::CreatePacket(
                        shared::networking::PacketTypes::ServerClientSetPlayerDetails));

        serverClientSetPlayerDetails->SetPlayerId(player->GetPlayerId());

        this->_packetSender->AddPacketToSend(player->GetNetworkClient()->GetSocket(), serverClientSetPlayerDetails);

        auto playerIt = std::find_if(this->_unauthenticatedPlayers.begin(), this->_unauthenticatedPlayers.end(),
                                  [&player](const auto& p)
                                  {
                                     return p->GetPlayerId() == player->GetPlayerId();
                                  });
        if (playerIt != this->_unauthenticatedPlayers.end())
        {
            this->_unauthenticatedPlayers.erase(playerIt);
        }

        this->_players.push_back(*playerIt);
    }

    void Server::HandleClientServerRequestHashedPasswordPacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                                               std::shared_ptr<engine::Player>& player) noexcept
    {
        auto clientServerRequestHashedPasswordPacket = std::static_pointer_cast<
                shared::networking::packets::ClientServerRequestHashedPasswordPacket>(packet);

        auto userName = clientServerRequestHashedPasswordPacket->GetUserName();

        std::string hashedPassword;
        if (!this->_dataManager->GetHashedPassword(userName, hashedPassword))
        {
            shared::api::logging::Log("Failed to get hashed password for username: " + userName);
            return;
        }

        const auto serverClientSendHashedPasswordPacket = std::static_pointer_cast<shared::networking::packets::ServerClientSendHashedPasswordPacket>(
                shared::networking::PacketFactory::CreatePacket(
                        shared::networking::PacketTypes::ServerClientSendHashedPassword));

        serverClientSendHashedPasswordPacket->SetUserName(userName);
        serverClientSendHashedPasswordPacket->SetHashedPassword(hashedPassword);

        this->_packetSender->AddPacketToSend(player->GetNetworkClient()->GetSocket(), serverClientSendHashedPasswordPacket);
    }
}
