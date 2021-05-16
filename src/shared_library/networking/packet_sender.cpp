#include "packet_sender.h"

namespace projectfarm::shared::networking
{
	bool PacketSender::Initialize() noexcept
	{
		this->LogMessage("Initializing packet sender...");

		if (this->_udpSocket == nullptr)
        {
            this->LogMessage("Creating UDP socket...");
            this->_udpSocket = SDLNet_UDP_Open(0);
            if (this->_udpSocket == nullptr)
            {
                this->LogMessage("Failed to create UDP socket.");
                this->LogMessage(SDLNet_GetError());
                return false;
            }
        }
		else
        {
		    this->LogMessage("Using external UDP socket.");
        }

        this->_udpPacket = SDLNet_AllocPacket(1024 * 100 * 100); // do 100kb for now
        if (this->_udpPacket == nullptr)
        {
            this->LogMessage("Failed to allow UDP packet.");
            return false;
        }

		this->_packetSenderWorker = std::make_unique<PacketSenderWorker>(this->_udpSocket, this->_udpPacket);
		this->_packetSenderWorker->SetLogger(this->_logger);

		this->_packetSenderWorker->StartThread();

		this->LogMessage("Initialized packet sender.");

		return true;
	}

	void PacketSender::Shutdown() noexcept
	{
		this->LogMessage("Shutting down packet sender...");

		if (this->_packetSenderWorker)
        {
            this->_packetSenderWorker->StopThread();
        }

		if (this->_internalUDPSocket)
        {
            SDLNet_UDP_Close(this->_udpSocket);
            this->_udpSocket = nullptr;
        }

        SDLNet_FreePacket(this->_udpPacket);
        this->_udpPacket = nullptr;

		this->LogMessage("Shut down packet sender.");
	}

	void PacketSender::AddPacketToSend(TCPsocket socket, const std::shared_ptr<Packet>& packet,
	        uint64_t milliseconds) noexcept
	{
        this->_packetSenderWorker->AddPacketToSend(socket, packet, milliseconds);
	}

    void PacketSender::AddPacketToSend(const IPaddress& ipAddress, const std::shared_ptr<Packet>& packet,
            uint64_t milliseconds) noexcept
    {
        this->_packetSenderWorker->AddPacketToSend(ipAddress, packet, milliseconds);
    }
}