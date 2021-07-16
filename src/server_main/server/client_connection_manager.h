#ifndef CLIENT_CONNECTION_MANAGER_H
#define CLIENT_CONNECTION_MANAGER_H

#include <string>
#include <atomic>
#include <mutex>
#include <vector>

#include <SDL_net.h>

#include "client.h"
#include "networking/packet_sender.h"
#include "client_connection_manager_worker.h"
#include "server_config.h"

namespace projectfarm::server
{
    class Server;

	class ClientConnectionManager final
	{
	public:
		ClientConnectionManager() = default;
		~ClientConnectionManager() = default;

		[[nodiscard]]
        bool Initialize(const std::shared_ptr<ServerConfig>& config) noexcept;

        void Shutdown() noexcept;

        // we have to pass the server in here rather than use `consume_server`
        // as otherwise on shutdown there will be memory leaks.
        // this seems to to be due to a cyclic shutdown dependency
        void Tick(const std::shared_ptr<Server>& server) noexcept;

    private:
        std::unique_ptr<ClientConnectionManagerWorker> _clientConnectionManagerWorker;

        std::atomic_bool _hasItemsToProcess {false};
        std::mutex _itemsMutex;

        void OnClientAdd(const std::shared_ptr<Client>& client) noexcept;
        void OnClientRemove(const std::shared_ptr<Client>& client) noexcept;
        void OnPacketReceive(const std::shared_ptr<shared::networking::Packet>& packet,
                             const std::shared_ptr<Client>& client) noexcept;
        void OnPacketReceive(const std::shared_ptr<shared::networking::Packet>& packet,
                             const IPaddress& ipAddress) noexcept;

        struct PacketToProcessType
        {
            std::shared_ptr<shared::networking::Packet> _packet;
            std::shared_ptr<Client> _client;
            IPaddress _ipAddress;
        };

        std::vector<std::shared_ptr<Client>> _clientsToAdd;
        std::vector<std::shared_ptr<Client>> _clientsToRemove;
        std::vector<PacketToProcessType> _packetsToProcess;
	};
}

#endif