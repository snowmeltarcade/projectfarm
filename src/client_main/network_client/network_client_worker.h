#ifndef PROJECTFARM_NETWORK_CLIENT_WORKER_H
#define PROJECTFARM_NETWORK_CLIENT_WORKER_H

#include <atomic>
#include <thread>

#include <SDL_net.h>

#include "logging/consume_logger.h"
#include "networking/packet.h"
#include "scenes/consume_scene_manager.h"
#include "networking/packet_receiver.h"

namespace projectfarm::network_client
{
    class NetworkClientWorker final : public projectfarm::shared::ConsumeLogger,
                                      public projectfarm::scenes::ConsumeSceneManager
    {
    public:
        NetworkClientWorker(TCPsocket tcpServerSocket, UDPsocket udpServerSocket,
                            SDLNet_SocketSet tcpSocketSet, UDPpacket* udpPacket)
            : _tcpServerSocket {tcpServerSocket},
              _udpServerSocket {udpServerSocket},
              _tcpSocketSet {tcpSocketSet},
              _udpPacket {udpPacket}
        {}
        ~NetworkClientWorker() override = default;

        void RunThread() noexcept;
        void StopThread() noexcept;
        
        void SetIsInLowerActivityState(bool state) noexcept
        {
            this->_isInLowerActivityState = state;
        }

    private:
        shared::networking::PacketReceiver _packetReceiver;

        TCPsocket _tcpServerSocket {nullptr};
        UDPsocket _udpServerSocket {nullptr};
        SDLNet_SocketSet _tcpSocketSet {nullptr};

        UDPpacket* _udpPacket {nullptr};

        std::atomic_bool _runThread {false};
        std::thread _thread;
        
        std::atomic_bool _isInLowerActivityState {false};

        void ThreadWorker() const noexcept;

        void CheckTCPSocket() const noexcept;
        void CheckUDPSocket() const noexcept;

        void HandleReceivedPacket(const std::shared_ptr<shared::networking::Packet>& packet) const noexcept;
    };
}

#endif
