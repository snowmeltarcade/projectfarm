#ifndef PROJECTFARM_CLIENT_CONNECTION_MANAGER_WORKER_H
#define PROJECTFARM_CLIENT_CONNECTION_MANAGER_WORKER_H

#include <atomic>
#include <thread>
#include <string>
#include <shared_mutex>
#include <vector>
#include <list>
#include <memory>
#include <functional>

#include <SDL_net.h>

#include "logging/consume_logger.h"
#include "client.h"
#include "networking/packet.h"
#include "networking/packet_receiver.h"

namespace projectfarm::server
{
    class ClientConnectionManagerWorker final : public projectfarm::shared::ConsumeLogger
    {
    public:
        using OnClientCallbackType = std::function<void(const std::shared_ptr<Client>&)>;
        using OnPacketReceiveTCPType = std::function<void(const std::shared_ptr<shared::networking::Packet>&,
                                                          const std::shared_ptr<Client>&)>;
        using OnPacketReceiveUDPType = std::function<void(const std::shared_ptr<shared::networking::Packet>&,
                                                          const IPaddress&)>;

        ClientConnectionManagerWorker(uint16_t tcpPort, uint16_t udpPort)
            : _tcpPort {tcpPort},
              _udpPort {udpPort}
        {}
        ~ClientConnectionManagerWorker() override = default;

        ClientConnectionManagerWorker(const ClientConnectionManagerWorker&) = delete;
        ClientConnectionManagerWorker(ClientConnectionManagerWorker&&) = delete;

        void RunThread();
        void StopThread();

        void SetOnClientAddCallback(const OnClientCallbackType& callback) noexcept
        {
            this->_onClientAddCallback = callback;
        }

        void SetOnClientRemoveCallback(const OnClientCallbackType& callback) noexcept
        {
            this->_onClientRemoveCallback = callback;
        }

        void SetOnPacketTCPCallback(const OnPacketReceiveTCPType& callback) noexcept
        {
            this->_onPacketReceiveTCP = callback;
        }

        void SetOnPacketUDPCallback(const OnPacketReceiveUDPType& callback) noexcept
        {
            this->_onPacketReceiveUDP = callback;
        }

    private:
        shared::networking::PacketReceiver _packetReceiver;

        uint16_t _tcpPort {0};
        uint16_t _udpPort {0};

        TCPsocket _tcpServerSocket {nullptr};
        UDPsocket _udpServerSocket {nullptr};
        SDLNet_SocketSet _tcpSocketSet {nullptr};

        UDPpacket* _udpPacket {nullptr};

        std::atomic<bool> _runThread {false};
        std::thread _thread;

        std::list<std::shared_ptr<Client>> _clients;

        std::vector<std::shared_ptr<Client>> _clientsToRemove;

        OnClientCallbackType _onClientAddCallback;
        OnClientCallbackType _onClientRemoveCallback;
        OnPacketReceiveTCPType _onPacketReceiveTCP;
        OnPacketReceiveUDPType _onPacketReceiveUDP;

        void ThreadWorker() noexcept;

        void CheckForNewClients() noexcept;
        void ProcessClients() noexcept;
        void ProcessClientsToRemove() noexcept;

        void ProcessClientsTCP() noexcept;
        void ProcessClientsUDP() noexcept;

        void OnPacketReceive(const std::shared_ptr<shared::networking::Packet>& packet,
                             const std::shared_ptr<Client>& client) noexcept;

        void OnPacketReceive(const std::shared_ptr<shared::networking::Packet>& packet,
                             const IPaddress& ipAddress) noexcept;

        [[nodiscard]]
        bool Initialize() noexcept;
        void Shutdown() noexcept;

        [[nodiscard]]
        bool CreateTCP();
        void ShutdownTCP();

        [[nodiscard]]
        bool CreateUDP();
        void ShutdownUDP();
    };
}

#endif
