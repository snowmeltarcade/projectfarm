#ifndef PACKET_SENDER_H
#define PACKET_SENDER_H

#include <memory>

#include "data/consume_data_provider.h"
#include "packet_sender_worker.h"

namespace projectfarm::shared::networking
{
	class PacketSender final : public ConsumeDataProvider
	{
	public:
		PacketSender() = default;
		~PacketSender() override
        {
		    this->Shutdown();
        }

		[[nodiscard]] bool Initialize() noexcept;
		void Shutdown() noexcept;

		void AddPacketToSend(TCPsocket socket, const std::shared_ptr<Packet>& packet,
                             uint64_t milliseconds = 0) noexcept;
        void AddPacketToSend(const IPaddress& ipAddress, const std::shared_ptr<Packet>& packet,
                             uint64_t milliseconds = 0) noexcept;

		void SetIsServer(bool isServer) noexcept
        {
		    this->_isServer = isServer;
        }

        void SetUDPSocket(UDPsocket udpSocket) noexcept
        {
		    this->_udpSocket = udpSocket;
            this->_internalUDPSocket = false;
        }
        
        void SetIsInLowerActivityState(bool state) noexcept
        {
            this->_packetSenderWorker->SetIsInLowerActivityState(state);
        }

	private:
		std::unique_ptr<PacketSenderWorker> _packetSenderWorker;

		UDPsocket _udpSocket {nullptr};
		bool _internalUDPSocket {true};
        UDPpacket* _udpPacket {nullptr};

        bool _isServer = true;
	};
}

#endif
