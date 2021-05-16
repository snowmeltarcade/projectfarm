#ifndef SERVER_H
#define SERVER_H

#include <memory>
#include <vector>
#include <list>
#include <tuple>
#include <cstdint>

#include "logging/logger.h"
#include "networking/networking.h"
#include "engine/world/world.h"
#include "networking/packet_sender.h"
#include "data/data_provider.h"
#include "system_arguments.h"
#include "server_config.h"
#include "client_connection_manager.h"
#include "engine/player.h"
#include "engine/player_load_details.h"
#include "scripting/script_system.h"
#include "engine/scripting/server_script_factory.h"
#include "math/random_engine.h"
#include "engine/entities/action_animations_manager.h"
#include "engine/data/data_manager.h"
#include "crypto/crypto_provider.h"

namespace projectfarm::server
{
	class Server final : public std::enable_shared_from_this<Server>
	{
	public:
		Server()
		{
			this->_logger = std::make_shared<projectfarm::shared::Logger>();
			this->_packetSender = std::make_shared<projectfarm::shared::networking::PacketSender>();
			this->_serverConfig = std::make_shared<ServerConfig>();
			this->_scriptSystem = std::make_shared<projectfarm::shared::scripting::ScriptSystem>();
			this->_scriptFactory = std::make_shared<engine::scripting::ServerScriptFactory>();
			this->_randomEngine = std::make_shared<projectfarm::shared::math::RandomEngine>();
			this->_actionAnimationsManager = std::make_shared<engine::entities::ActionAnimationsManager>();
			this->_dataManager = std::make_shared<engine::data::DataManager>();
			this->_cryptoProvider = std::make_shared<projectfarm::shared::crypto::CryptoProvider>();
		}
		~Server() = default;

		Server(Server&) = delete;
		Server(Server&&) = delete;

		[[nodiscard]] std::shared_ptr<Server> GetPtr()
		{
			return this->shared_from_this();
		}

		void Run(int argc, char* argv[]);

		void OnClientAdd(const std::shared_ptr<Client>& client) noexcept;
        void OnClientRemove(const std::shared_ptr<Client>& client) noexcept;
		void OnReceivePacket(const std::shared_ptr<shared::networking::Packet>& packet,
                             const std::shared_ptr<Client>& client,
                             const IPaddress& ipAddress) noexcept;

		bool AddPlayerToWorld(uint32_t playerId, uint32_t entityId,
                              const std::string& destinationWorldName) noexcept;

		bool AddCharacterToWorld(const std::string& characterType,
                                 const std::string& destinationWorldName,
                                 const std::string& destinationTileType,
                                 uint32_t entityId, uint32_t playerId) noexcept;

		[[nodiscard]] std::shared_ptr<engine::Player> GetPlayerById(uint32_t playerId) const noexcept;

	private:
	    SystemArguments _systemArguments;
	    std::shared_ptr<ServerConfig> _serverConfig;

	    ClientConnectionManager _clientConnectionManager;

		projectfarm::shared::networking::Networking _networking;
		std::shared_ptr<projectfarm::shared::Logger> _logger;
		std::shared_ptr<projectfarm::shared::networking::PacketSender> _packetSender;
		std::shared_ptr<projectfarm::shared::DataProvider> _dataProvider;
		std::shared_ptr<projectfarm::shared::scripting::ScriptSystem> _scriptSystem;
		std::shared_ptr<engine::scripting::ServerScriptFactory> _scriptFactory;
		std::shared_ptr<projectfarm::shared::math::RandomEngine> _randomEngine;
        std::shared_ptr<engine::entities::ActionAnimationsManager> _actionAnimationsManager;
        std::shared_ptr<engine::data::DataManager> _dataManager;
        std::shared_ptr<projectfarm::shared::crypto::CryptoProvider> _cryptoProvider;

		[[nodiscard]] bool CreateWorld(const std::string& name, const std::filesystem::path& worldFilePath);
		[[nodiscard]] bool CreateWorlds();

		std::vector<std::shared_ptr<engine::world::World>> _worlds;
		std::vector<std::shared_ptr<engine::Player>> _players;
        std::vector<std::shared_ptr<engine::Player>> _unauthenticatedPlayers;

		bool _shouldQuit = false;

		bool Initialize();

		void MainLoop();
        void HandleEvents();
        void UpdateWorlds();

		void TellServerToQuit();
		void Shutdown();

		[[nodiscard]] std::shared_ptr<engine::Player> FindPlayer(const std::shared_ptr<shared::networking::Packet>& packet,
                                                                 const std::shared_ptr<Client>& client) const noexcept;

        [[nodiscard]] std::shared_ptr<engine::Player> FindPlayer(const std::shared_ptr<shared::networking::Packet>& packet,
                                                                 const std::shared_ptr<Client>& client,
                                                                 const std::vector<std::shared_ptr<engine::Player>>& playersList) const noexcept;

		void HandleClientServerTestUdpPacket(const std::shared_ptr<shared::networking::Packet>& packet,
		                                     const IPaddress& ipAddress,
		                                     const std::shared_ptr<engine::Player>& player) noexcept;

        void HandleClientServerPlayerAuthenticatePacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                                        std::shared_ptr<engine::Player>& player) noexcept;

        void HandleClientServerRequestHashedPasswordPacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                                           std::shared_ptr<engine::Player>& player) noexcept;

		[[nodiscard]] std::shared_ptr<engine::PlayerLoadDetails> GetPlayerLoadDetails(uint32_t playerId) const noexcept;

		static void InitializePlayer(std::shared_ptr<engine::Player>& player,
                              const std::shared_ptr<engine::PlayerLoadDetails>& loadDetails) noexcept;
	};
}

#endif