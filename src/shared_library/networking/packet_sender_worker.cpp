#include <cstring>

#include "packet_sender_worker.h"
#include "api/logging/logging.h"

using namespace std::literals;

namespace projectfarm::shared::networking
{
    void PacketSenderWorker::StartThread() noexcept
    {
        api::logging::Log("Starting packet sender thread...");

        this->_runThread = true;

        this->_thread = std::thread(&PacketSenderWorker::ThreadWorker, this);

        api::logging::Log("Started packet sender thread.");
    }

    void PacketSenderWorker::StopThread() noexcept
    {
        api::logging::Log("Stopping packet sender thread...");

        if (this->_thread.joinable())
        {
            this->_runThread = false;

            // we are checking if the thread quits in this CV
            this->_packetMutexCV.notify_one();

            this->_thread.join();
        }

        api::logging::Log("Stopped packet sender thread.");
    }

    void PacketSenderWorker::ThreadWorker() noexcept
    {
        api::logging::Log("In packet sender thread.");

        this->_currentTime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

        while (this->_runThread)
        {
            if (this->_isInLowerActivityState)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            {
                std::unique_lock lock(this->_packetMutex);
                this->_packetMutexCV.wait(lock,
                                          [this]() { return !this->_packetsToSend.empty() ||
                                                                  !this->_runThread; });

                if (!this->_runThread)
                {
                    break;
                }

                while (!this->_packetsToSend.empty())
                {
                    auto packet = this->_packetsToSend.front();
                    this->_packetsToSend.pop();

                    this->SendPacket(packet);
                }

                for (auto packetIter = this->_countdownPacketsToSend.begin();
                     packetIter != this->_countdownPacketsToSend.end();)
                {
                    auto &packet = *packetIter;

                    int64_t milliseconds = packet._milliseconds - this->_lastFrameTime;
                    if (milliseconds <= 0)
                    {
                        this->_packetsToSend.push(packet);

                        packetIter = this->_countdownPacketsToSend.erase(packetIter);
                        continue;
                    }

                    ++packetIter;

                    packet._milliseconds -= this->_lastFrameTime;
                }
            }

            auto currentTime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count());
            this->_lastFrameTime = currentTime - this->_currentTime;
            this->_currentTime = currentTime;
        }

        api::logging::Log("Exiting packet sender thread.");
    }

    void PacketSenderWorker::AddPacketToSend(TCPsocket socket, const std::shared_ptr<Packet>& packet,
                                             uint64_t milliseconds) noexcept
    {
        {
            std::unique_lock lock(this->_packetMutex);

            if (milliseconds > 0)
            {
                this->_countdownPacketsToSend.push_back({ socket, packet, milliseconds });
            }
            else
            {
                this->_packetsToSend.push({ socket, packet, 0 });
            }
        }

        this->_packetMutexCV.notify_one();
    }

    void PacketSenderWorker::AddPacketToSend(const IPaddress& ipAddress, const std::shared_ptr<Packet>& packet,
                                             uint64_t milliseconds) noexcept
    {
        {
            std::unique_lock lock(this->_packetMutex);

            if (milliseconds > 0)
            {
                this->_countdownPacketsToSend.push_back({ ipAddress, packet, milliseconds });
            }
            else
            {
                this->_packetsToSend.push({ ipAddress, packet, 0 });
            }
        }

        this->_packetMutexCV.notify_one();
    }

    // TODO: Put this somewhere more appropriate...
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    // C++20 doesn't need this
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    void PacketSenderWorker::SendPacket(PacketSendInfo& info) noexcept
    {
        std::visit(overloaded {
           [this, &info](const IPaddress& ipAddress)
           {
               this->_udpPacket->len = info._packet->PacketSize();

               auto bytes = info._packet->GetBytes();
               std::memcpy(static_cast<void *>(this->_udpPacket->data), static_cast<void *>(&bytes.front()), this->_udpPacket->len);

               this->_udpPacket->address.host = ipAddress.host;
               this->_udpPacket->address.port = ipAddress.port;

               if (!SDLNet_UDP_Send(this->_udpSocket, -1, this->_udpPacket))
               {
                   // TODO: Find out how to get destination IP address from the socket, then log it below
                   api::logging::Log("Failed to send message (UDP).");
                   api::logging::Log(SDLNet_GetError());
               }
           },
           [&info](const TCPsocket& socket)
           {
               auto bytes = info._packet->GetBytes();
               auto size = info._packet->PacketSize();

               if (!socket)
               {
                   // something must have gone wrong - perhaps the player left?
                   return;
               }

               auto bytesSent = SDLNet_TCP_Send(socket, static_cast<void *>(&bytes.front()), size * sizeof(uint8_t));
               if (static_cast<uint32_t>(bytesSent) < size)
               {
                   // TODO: Find out how to get destination IP address from the socket, then log it below
                   api::logging::Log("Failed to send message (TCP).");
                   api::logging::Log(SDLNet_GetError());
               }
           }
        },
        info._destination);
    }
}
