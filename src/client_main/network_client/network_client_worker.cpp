#include <vector>

#include "network_client_worker.h"
#include "networking/packet_factory.h"
#include "scenes/scene_manager.h"
#include "api/logging/logging.h"

namespace projectfarm::network_client
{
    void NetworkClientWorker::RunThread() noexcept
    {
        shared::api::logging::Log("Running worker thread.");

        this->_runThread = true;

        this->_thread = std::thread(&NetworkClientWorker::ThreadWorker, this);

        shared::api::logging::Log("Worker thread running.");
    }

    void NetworkClientWorker::StopThread() noexcept
    {
        shared::api::logging::Log("Stopping worker thread.");

        if (this->_thread.joinable())
        {
            this->_runThread = false;
            this->_thread.join();
        }

        shared::api::logging::Log("Worker thread stopped.");
    }

    void NetworkClientWorker::ThreadWorker() const noexcept
    {
        shared::api::logging::Log("In worker thread.");

        while (this->_runThread)
        {
            if (this->_isInLowerActivityState)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            
            this->CheckTCPSocket();
            this->CheckUDPSocket();

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        shared::api::logging::Log("Returning from worker thread.");
    }

    void NetworkClientWorker::CheckTCPSocket() const noexcept
    {
        if (SDLNet_CheckSockets(this->_tcpSocketSet, 0) > 0)
        {
            auto [success, packet] = this->_packetReceiver.CheckTCPSocket(this->_tcpServerSocket);

            if (!success)
            {
                // TODO: Handle this properly
                shared::api::logging::Log("Server disconnected???");
                return;
            }

            if (packet)
            {
                this->HandleReceivedPacket(packet);
            }
        }
    }

    void NetworkClientWorker::CheckUDPSocket() const noexcept
    {
        auto packet = this->_packetReceiver.CheckUDPSocket(this->_udpServerSocket, this->_udpPacket);
        if (packet == nullptr)
        {
            return;
        }

        this->HandleReceivedPacket(packet);
    }

    void NetworkClientWorker::HandleReceivedPacket(const std::shared_ptr<shared::networking::Packet>& packet) const noexcept
    {
#ifdef LOGGING_PACKET_DEBUG_INFO
        //const auto debugData = packet->GetDebugData();
        //shared::api::logging::Log(debugData);
#endif

        auto sceneManager = this->GetSceneManager();
        sceneManager->PassPacketToCurrentScene(packet);
    }
}
