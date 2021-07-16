#include "client_connection_manager.h"
#include "server.h"
#include "api/logging/logging.h"

namespace projectfarm::server
{
	bool ClientConnectionManager::Initialize(const std::shared_ptr<ServerConfig>& config) noexcept
	{
		shared::api::logging::Log("Initializing client connection manager.");

		this->_clientConnectionManagerWorker = std::make_unique<ClientConnectionManagerWorker>(config->GetTcpPort(),
                                                                                               config->GetServerUdpPort());

		this->_clientConnectionManagerWorker->SetOnClientAddCallback([this](const auto& client)
                                                                     { this->OnClientAdd(client); });

        this->_clientConnectionManagerWorker->SetOnClientRemoveCallback([this](const auto& client)
                                                                        { this->OnClientRemove(client); });

        this->_clientConnectionManagerWorker->SetOnPacketTCPCallback([this](const auto& packet, const auto& client)
                                                                    { this->OnPacketReceive(packet, client); });

        this->_clientConnectionManagerWorker->SetOnPacketUDPCallback([this](const auto& packet, const auto& ipAddress)
                                                                     { this->OnPacketReceive(packet, ipAddress); });

		this->_clientConnectionManagerWorker->RunThread();

		shared::api::logging::Log("Initialized client connection manager");

		return true;
	}

	void ClientConnectionManager::Shutdown() noexcept
	{
		shared::api::logging::Log("Shutting down client connection manager");

		if (this->_clientConnectionManagerWorker)
        {
            this->_clientConnectionManagerWorker->StopThread();
        }

		shared::api::logging::Log("Shut down session client manager");
	}

    void ClientConnectionManager::Tick(const std::shared_ptr<Server>& server) noexcept
    {
	    if (!this->_hasItemsToProcess)
        {
	        return;
        }

	    std::scoped_lock<std::mutex> lock(this->_itemsMutex);

	    for (const auto& client : this->_clientsToAdd)
        {
	        shared::api::logging::Log("Added client: " + client->IPAddressAsString());

	        server->OnClientAdd(client);
        }

	    this->_clientsToAdd.clear();

        for (const auto& client : this->_clientsToRemove)
        {
            shared::api::logging::Log("Removed client: " + client->IPAddressAsString());

            server->OnClientRemove(client);
        }

        this->_clientsToRemove.clear();

        for (const auto& item : this->_packetsToProcess)
        {
            server->OnReceivePacket(item._packet, item._client, item._ipAddress);
        }

        this->_packetsToProcess.clear();

	    this->_hasItemsToProcess = false;
    }

    void ClientConnectionManager::OnClientAdd(const std::shared_ptr<Client> &client) noexcept
    {
        std::scoped_lock<std::mutex> lock(this->_itemsMutex);

        this->_clientsToAdd.push_back(client);

        this->_hasItemsToProcess = true;
    }

    void ClientConnectionManager::OnClientRemove(const std::shared_ptr<Client> &client) noexcept
    {
        std::scoped_lock<std::mutex> lock(this->_itemsMutex);

        this->_clientsToRemove.push_back(client);

        this->_hasItemsToProcess = true;
    }

    void ClientConnectionManager::OnPacketReceive(const std::shared_ptr<shared::networking::Packet>& packet,
                                                  const std::shared_ptr<Client>& client) noexcept
    {
        std::scoped_lock<std::mutex> lock(this->_itemsMutex);

        this->_packetsToProcess.push_back({packet, client, {}});

        this->_hasItemsToProcess = true;
    }

    void ClientConnectionManager::OnPacketReceive(const std::shared_ptr<shared::networking::Packet>& packet,
                                                  const IPaddress& ipAddress) noexcept
    {
        std::scoped_lock<std::mutex> lock(this->_itemsMutex);

        this->_packetsToProcess.push_back({packet, nullptr, ipAddress});

        this->_hasItemsToProcess = true;
    }
}
