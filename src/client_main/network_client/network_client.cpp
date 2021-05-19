#include "network_client.h"

namespace projectfarm::network_client
{
	using namespace std::chrono_literals;

	bool NetworkClient::Initialize(const std::string& hostName, uint16_t tcpPort,
                                   uint16_t clientUdpPort, uint16_t serverUdpPort)
	{
		this->LogMessage("Initializing network client...");

		this->LogMessage("Resolving server address (TCP): " + hostName + ":" + std::to_string(tcpPort));
		if (SDLNet_ResolveHost(&this->_tcpServerIP, hostName.c_str(), tcpPort) < 0)
		{
			this->LogMessage("Failed to resolve server address (TCP).");
			this->LogMessage(SDLNet_GetError());
			return false;
		}
		this->LogMessage("Resolved server address (TCP).");

        this->LogMessage("Resolving server address (UDP): " + hostName + ":" + std::to_string(serverUdpPort));
        if (SDLNet_ResolveHost(&this->_udpServerIP, hostName.c_str(), serverUdpPort) < 0)
        {
            this->LogMessage("Failed to resolve server address (UDP).");
            this->LogMessage(SDLNet_GetError());
            return false;
        }
        this->LogMessage("Resolved server address (UDP).");

		this->LogMessage("Connecting to server over TCP...");
		this->_tcpServerSocket = SDLNet_TCP_Open(&this->_tcpServerIP);
		if (this->_tcpServerSocket == nullptr)
		{
			this->LogMessage("Failed to connect to server over TCP.");
			this->LogMessage(SDLNet_GetError());
			return false;
		}

        this->LogMessage("Creating UDP socket on port " + std::to_string(clientUdpPort) + "...");
        this->_udpServerSocket = SDLNet_UDP_Open(clientUdpPort);
        if (this->_udpServerSocket == nullptr)
        {
            this->LogMessage("Failed to create UDP socket.");
            this->LogMessage(SDLNet_GetError());
            return false;
        }

        auto udpIPAddress = SDLNet_UDP_GetPeerAddress(this->_udpServerSocket, -1);
        if (udpIPAddress == nullptr)
        {
            this->LogMessage("Failed to get UDP address.");
        }
        else
        {
            this->LogMessage("UDP connected to host: " + std::to_string(udpIPAddress->host) +
                                " and port: " + std::to_string(udpIPAddress->port));
        }

		this->LogMessage("Connected to server.");

		this->_tcpSocketSet = SDLNet_AllocSocketSet(1);
		SDLNet_TCP_AddSocket(this->_tcpSocketSet, this->_tcpServerSocket);

		this->_packetSender->SetLogger(this->_logger);
        this->_packetSender->SetUDPSocket(this->_udpServerSocket);
		if (!this->_packetSender->Initialize())
		{
			this->_logger->LogMessage("Failed to initialize packet sender.");
			return false;
		}

		this->_udpPacket = SDLNet_AllocPacket(1024 * 100 * 100); // do 100kb for now
		if (this->_udpPacket == nullptr)
        {
		    this->LogMessage("Failed to allow UDP packet.");
		    return false;
        }

		this->_udpPacket->address.host = this->_udpServerIP.host;
		this->_udpPacket->address.port = this->_udpServerIP.port;

		this->_networkClientWorker = std::make_unique<NetworkClientWorker>(
		        this->_tcpServerSocket, this->_udpServerSocket,
		        this->_tcpSocketSet, this->_udpPacket);

		this->_networkClientWorker->SetLogger(this->_logger);
		this->_networkClientWorker->SetSceneManager(this->GetSceneManager());

		this->_networkClientWorker->RunThread();

		this->LogMessage("Initialized network client.");

		return true;
	}

	void NetworkClient::Shutdown()
	{
		if (!this->IsConnected())
        {
            return;
        }

		this->_packetSender->Shutdown();

        this->_networkClientWorker->StopThread();

		SDLNet_FreeSocketSet(this->_tcpSocketSet);
		this->_tcpSocketSet = nullptr;

		SDLNet_TCP_Close(this->_tcpServerSocket);
		this->_tcpServerSocket = nullptr;

        SDLNet_UDP_Close(this->_udpServerSocket);
        this->_udpServerSocket = nullptr;

        SDLNet_FreePacket(this->_udpPacket);
        this->_udpPacket = nullptr;
	}
}