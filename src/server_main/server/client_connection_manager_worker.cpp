#include "client_connection_manager_worker.h"

#include "networking/packet_factory.h"

namespace projectfarm::server
{
    void ClientConnectionManagerWorker::RunThread()
    {
        this->LogMessage("Running worker thread for client connection manager");

        this->_packetReceiver.SetLogger(this->_logger);

        this->_runThread = true;

        this->_thread = std::thread(&ClientConnectionManagerWorker::ThreadWorker, this);

        this->LogMessage("Worker thread running for client connection manager");
    }

    void ClientConnectionManagerWorker::StopThread()
    {
        this->LogMessage("Stopping worker thread for client connection manager");

        if (this->_thread.joinable())
        {
            this->_runThread = false;
            this->_thread.join();
        }

        this->LogMessage("Worker thread stopped for client connection manager");
    }

    void ClientConnectionManagerWorker::ThreadWorker() noexcept
    {
        this->LogMessage("In worker thread for client connection manager");

        if (!this->Initialize())
        {
            this->LogMessage("Failed to initialize client connection manager worker.");
            return;
        }

        while (this->_runThread)
        {
            this->CheckForNewClients();

            this->ProcessClients();

            this->ProcessClientsToRemove();

            std::this_thread::yield();
        }

        this->Shutdown();

        this->LogMessage("Returning from worker thread for client connection manager");
    }

    void ClientConnectionManagerWorker::CheckForNewClients() noexcept
    {
        TCPsocket clientSocket = SDLNet_TCP_Accept(this->_tcpServerSocket);
        if (clientSocket)
        {
            IPaddress* clientIP = SDLNet_TCP_GetPeerAddress(clientSocket);

            std::shared_ptr<Client> client = std::make_shared<Client>(clientIP, clientSocket);

            this->LogMessage("New client: " + client->IPAddressAsString());

            // TODO: Check we have not already added all clients allocated for this socket set
            SDLNet_TCP_AddSocket(this->_tcpSocketSet, client->GetSocket());

            if (this->_onClientAddCallback)
            {
                this->_onClientAddCallback(client);
            }

            this->_clients.emplace_back(std::move(client));
        }
    }

    void ClientConnectionManagerWorker::ProcessClients() noexcept
    {
        this->ProcessClientsTCP();
        this->ProcessClientsUDP();
    }

    void ClientConnectionManagerWorker::ProcessClientsTCP() noexcept
    {
        int numberOfSocketsReady = SDLNet_CheckSockets(this->_tcpSocketSet, 0);
        if (numberOfSocketsReady > 0)
        {
            for (auto& client : this->_clients)
            {
                auto [success, packet] = this->_packetReceiver.CheckTCPSocket(client->GetSocket());

                if (!success)
                {
                    this->_clientsToRemove.push_back(client);
                    continue;
                }

                if (packet)
                {
                    this->OnPacketReceive(packet, client);
                }
            }
        }
    }

    void ClientConnectionManagerWorker::ProcessClientsUDP() noexcept
    {
        auto packet = this->_packetReceiver.CheckUDPSocket(this->_udpServerSocket, this->_udpPacket);
        if (packet == nullptr)
        {
            return;
        }

        this->OnPacketReceive(packet, this->_udpPacket->address);
    }

    void ClientConnectionManagerWorker::ProcessClientsToRemove() noexcept
    {
        for (const auto& client : this->_clientsToRemove)
        {
            this->LogMessage("Removing client: " + client->IPAddressAsString());

            if (this->_onClientRemoveCallback)
            {
                this->_onClientRemoveCallback(client);
            }

            SDLNet_TCP_DelSocket(this->_tcpSocketSet, client->GetSocket());

            client->Disconnect();

            this->_clients.remove(client);
        }

        this->_clientsToRemove.clear();
    }

    void ClientConnectionManagerWorker::OnPacketReceive(const std::shared_ptr<shared::networking::Packet>& packet,
                                                        const std::shared_ptr<Client>& client) noexcept
    {
        if (this->_onPacketReceiveTCP)
        {
            this->_onPacketReceiveTCP(packet, client);
        }
    }

    void ClientConnectionManagerWorker::OnPacketReceive(const std::shared_ptr<shared::networking::Packet>& packet,
                                                        const IPaddress& ipAddress) noexcept
    {
        if (this->_onPacketReceiveUDP)
        {
            this->_onPacketReceiveUDP(packet, ipAddress);
        }
    }

    bool ClientConnectionManagerWorker::Initialize() noexcept
    {
        this->LogMessage("Initializing client connection manager worker...");

        if (!this->CreateTCP())
        {
            this->LogMessage("Failed to initialize TCP.");
            return false;
        }

        if (!this->CreateUDP())
        {
            this->LogMessage("Failed to initialize UDP.");
            return false;
        }

        this->_tcpSocketSet = SDLNet_AllocSocketSet(16);
        if (!this->_tcpSocketSet)
        {
            this->LogMessage("Failed to allocate socket set.");
            this->LogMessage(SDLNet_GetError());
            return false;
        }

        this->LogMessage("Initialized client connection manager worker.");

        return true;
    }

    void ClientConnectionManagerWorker::Shutdown() noexcept
    {
        this->LogMessage("Shutting down client connection manager worker...");

        for (auto client : this->_clients)
        {
            this->_clientsToRemove.push_back(client);
        }

        this->ProcessClientsToRemove();

        this->_clientsToRemove.clear();
        this->_clients.clear();

        if (this->_tcpSocketSet)
        {
            SDLNet_FreeSocketSet(this->_tcpSocketSet);
            this->_tcpSocketSet = nullptr;
        }

        this->ShutdownTCP();
        this->ShutdownUDP();

        this->LogMessage("Shut down client connection manager worker.");
    }

    bool ClientConnectionManagerWorker::CreateTCP()
    {
        this->LogMessage("Creating TCP...");

        IPaddress localServerIP;

        this->LogMessage("Binding to port: " + std::to_string(this->_tcpPort));

        if (SDLNet_ResolveHost(&localServerIP, nullptr, this->_tcpPort) < 0)
        {
            this->LogMessage("Failed to resolve the TCP server host:");
            this->LogMessage(SDLNet_GetError());
            return false;
        }

        this->_tcpServerSocket = SDLNet_TCP_Open(&localServerIP);
        if (this->_tcpServerSocket == nullptr)
        {
            this->LogMessage("Failed to open the TCP server socket: " + std::to_string(this->_tcpPort));
            this->LogMessage(SDLNet_GetError());
            return false;
        }

        return true;
    }

    void ClientConnectionManagerWorker::ShutdownTCP()
    {
        SDLNet_TCP_Close(this->_tcpServerSocket);
        this->_tcpServerSocket = nullptr;
    }

    bool ClientConnectionManagerWorker::CreateUDP()
    {
        this->LogMessage("Creating UDP...");

        this->_udpServerSocket = SDLNet_UDP_Open(this->_udpPort);
        if (!this->_udpServerSocket)
        {
            this->LogMessage("Failed to open the UDP server socket: " + std::to_string(this->_udpPort));
            this->LogMessage(SDLNet_GetError());
            return false;
        }

        this->_udpPacket = SDLNet_AllocPacket(1024 * 100 * 100); // do 100kb for now
        if (!this->_udpPacket)
        {
            this->LogMessage("Failed to allow UDP packet.");
            return false;
        }

        return true;
    }

    void ClientConnectionManagerWorker::ShutdownUDP()
    {
        SDLNet_UDP_Close(this->_udpServerSocket);
        this->_udpServerSocket = nullptr;

        SDLNet_FreePacket(this->_udpPacket);
        this->_udpPacket = nullptr;
    }
}