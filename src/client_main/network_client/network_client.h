#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <memory>
#include <string>
#include <cstdint>
#include <SDL_net.h>

#include "networking/packet_sender.h"
#include "scenes/consume_scene_manager.h"
#include "network_client_worker.h"

namespace projectfarm::network_client
{
	class NetworkClient final : public projectfarm::scenes::ConsumeSceneManager,
						        public std::enable_shared_from_this<NetworkClient>
	{
	public:
		NetworkClient()
		{
			this->_packetSender = std::make_shared<projectfarm::shared::networking::PacketSender>();
			this->_packetSender->SetIsServer(false);
		}
		~NetworkClient() override
        {
		    this->Shutdown();
        }

		[[nodiscard]] bool Initialize(const std::string& hostName, uint16_t tcpPort,
		                              uint16_t clientUdpPort, uint16_t serverUdpPort);

		void Shutdown();

		[[nodiscard]] bool IsConnected() const noexcept
        {
            return this->_tcpServerSocket != nullptr;
        }

		[[nodiscard]] auto GetPacketSender() const noexcept
		    -> std::shared_ptr<projectfarm::shared::networking::PacketSender>
		{
			return this->_packetSender;
		}

		[[nodiscard]] TCPsocket GetTcpServerSocket() const noexcept
		{
			return this->_tcpServerSocket;
		}

        [[nodiscard]] const IPaddress& GetUdpServerIPAddress() const noexcept
        {
		    return this->_udpServerIP;
        }
        
        void SetIsInLowerActivityState(bool state) noexcept
        {
            this->_networkClientWorker->SetIsInLowerActivityState(state);
            this->_packetSender->SetIsInLowerActivityState(state);
        }

	private:
		IPaddress _tcpServerIP {};
        IPaddress _udpServerIP {};
		TCPsocket _tcpServerSocket {nullptr};
        UDPsocket _udpServerSocket {nullptr};
		SDLNet_SocketSet _tcpSocketSet {nullptr};
        UDPpacket* _udpPacket {nullptr};

        std::unique_ptr<NetworkClientWorker> _networkClientWorker;

		std::shared_ptr<projectfarm::shared::networking::PacketSender> _packetSender;
	};
}

#endif
