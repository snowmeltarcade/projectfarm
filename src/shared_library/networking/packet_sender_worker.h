#ifndef PROJECTFARM_PACKET_SENDER_WORKER_H
#define PROJECTFARM_PACKET_SENDER_WORKER_H

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <list>
#include <variant>

#include <SDL_net.h>

#include "logging/consume_logger.h"
#include "networking/packet.h"
#include "utils/util.h"

namespace projectfarm::shared::networking
{
    class PacketSenderWorker final : public ConsumeLogger
    {
    public:
        PacketSenderWorker(UDPsocket udpSocket, UDPpacket* udpPacket)
        : _udpSocket {udpSocket},
          _udpPacket {udpPacket}
        {}
        ~PacketSenderWorker() override = default;

        PacketSenderWorker(const PacketSenderWorker&) = delete;
        PacketSenderWorker(PacketSenderWorker&&) = delete;

        void StartThread() noexcept;
        void StopThread() noexcept;

        void AddPacketToSend(TCPsocket socket, const std::shared_ptr<Packet>& packet,
                uint64_t milliseconds = 0) noexcept;
        void AddPacketToSend(const IPaddress& ipAddress, const std::shared_ptr<Packet>& packet,
                uint64_t milliseconds = 0) noexcept;
        
        void SetIsInLowerActivityState(bool state) noexcept
        {
            this->_isInLowerActivityState = state;
        }

    private:
        std::atomic_bool _runThread {false};
        std::thread _thread;
        
        std::atomic_bool _isInLowerActivityState {false};

        std::mutex _packetMutex;
        std::condition_variable _packetMutexCV;

        uint64_t _currentTime {0};
        uint64_t _lastFrameTime {0};

        struct PacketSendInfo
        {
            std::variant<TCPsocket, IPaddress> _destination;
            std::shared_ptr<Packet> _packet;
            uint64_t _milliseconds;

            bool operator == (const PacketSendInfo& other) const
            {
                return this->_destination == other._destination && this->_packet == other._packet;
            }

            bool operator != (const PacketSendInfo& other) const
            {
                return !(*this == other);
            }
        };

        std::queue<PacketSendInfo> _packetsToSend;
        std::list<PacketSendInfo> _countdownPacketsToSend;

        UDPsocket _udpSocket {nullptr};
        UDPpacket* _udpPacket {nullptr};

        void ThreadWorker() noexcept;

        void SendPacket(PacketSendInfo& info) noexcept;
    };
}

#endif
