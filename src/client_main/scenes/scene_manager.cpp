#include <string>

#include "engine/game.h"
#include "implemented_scenes/loading_scene.h"
#include "implemented_scenes/world_scene.h"
#include "implemented_scenes/authenticate_scene.h"
#include "networking/packet_factory.h"
#include "networking/packets/client_server_entity_update.h"
#include "api/logging/logging.h"

namespace projectfarm::scenes
{
    void SceneManager::QueueLoadScene(SceneTypes type)
    {
        this->_queuedSceneType = std::make_optional<projectfarm::scenes::SceneTypes>(type);
    }

    bool SceneManager::HandleQueuedScene(const std::function<void(void)>& onSceneLoadHandler)
    {
        if (!this->_queuedSceneType.has_value())
        {
            return true;
        }

        auto result = this->LoadScene(*this->_queuedSceneType);

        this->_queuedSceneType = std::nullopt;

        if (result)
        {
            onSceneLoadHandler();
        }

        return result;
    }

	bool SceneManager::LoadScene(SceneTypes type)
	{
        auto& player = this->GetGame()->GetPlayer();
        if (auto character = player.GetCharacter(); character)
        {
            character->Shutdown();
        }
        player.SetCharacter(nullptr);

		if (this->_currentScene != nullptr)
		{
		    this->_currentScene->Destroy();
			this->_currentScene = nullptr;
		}

		this->_currentScene = this->CreateSceneBySceneType(type);
		this->_currentScene->SetSceneManager(this->shared_from_this());
		this->_currentScene->SetGraphics(this->GetGame()->GetGraphics());
		this->_currentScene->SetDataProvider(this->GetGame()->GetDataProvider());
		this->_currentScene->SetFontManager(this->GetGame()->GetFontManager());
		this->_currentScene->SetRenderManager(this->GetGame()->GetGraphics()->GetRenderManager());
		this->_currentScene->SetTimer(this->GetTimer());
		this->_currentScene->SetDebugInformation(this->GetDebugInformation());
        this->_currentScene->SetCryptoProvider(this->GetGame()->GetCryptoProvider());
        this->_currentScene->SetScriptSystem(this->_scriptSystem);
		this->_currentScene->SetGame(this->GetGame());

		this->_currentScene->SetLoadParameters(this->_sceneLoadParameters);

		if (!this->_currentScene->Initialize())
		{
			shared::api::logging::Log("Failed to load scene with type: " + std::to_string(static_cast<int>(type)));
			return false;
		}

		this->_sceneLoadParameters.clear();

		return true;
	}

	std::shared_ptr<Scene> SceneManager::CreateSceneBySceneType(SceneTypes type) const
	{
        std::shared_ptr<Scene> scene;

		switch (type)
		{
            case SceneTypes::Loading:
            {
                scene = std::make_shared<implemented_scenes::LoadingScene>();
                break;
            }
            case SceneTypes::WorldScene:
            {
                scene = std::make_shared<implemented_scenes::WorldScene>();
                break;
            }
            case SceneTypes::AuthenticateScene:
            {
                scene = std::make_shared<implemented_scenes::AuthenticateScene>();
                break;
            }
		}

		if (!scene)
        {
            shared::api::logging::Log("Unknown scene type: " + std::to_string(static_cast<int>(type)));
            return {};
        }

		return scene;
	}

	void SceneManager::PassPacketToCurrentScene(const std::shared_ptr<shared::networking::Packet>& packet)
	{
		std::scoped_lock<std::mutex> lock(this->_networkPacketMutex);
		this->_hasNetworkPacketsToProcess = true;

		this->_networkPacketsToSendToCurrentScene.emplace_back(packet);
	}

	void SceneManager::Tick()
	{
		this->HandleNetworkPackets();

		this->_currentScene->Tick();
	}

	void SceneManager::HandleNetworkPackets()
	{
		// avoid locking a mutex if not needed
		if (!this->_hasNetworkPacketsToProcess)
		{
			return;
		}

		std::scoped_lock<std::mutex> lock(this->_networkPacketMutex);

		for (const auto& packet : this->_networkPacketsToSendToCurrentScene)
		{
			this->_currentScene->ProcessPacket(packet);
		}

		this->_networkPacketsToSendToCurrentScene.clear();
		this->_hasNetworkPacketsToProcess = false;
	}

    void SceneManager::SendPacketToServer(const std::shared_ptr<shared::networking::Packet>& packet) const noexcept
    {
        auto packetSender = this->GetNetworkClient()->GetPacketSender();

        if (packet->IsVital())
        {
            packetSender->AddPacketToSend(this->GetNetworkClient()->GetTcpServerSocket(), packet);
        }
        else
        {
            packetSender->AddPacketToSend(this->GetNetworkClient()->GetUdpServerIPAddress(), packet);
        }
    }

    void SceneManager::BroadcastEntityState(const std::shared_ptr<engine::entities::Entity>& entity,
            uint64_t currentTime) const noexcept
    {
        const auto clientServerEntityUpdatePacket = std::static_pointer_cast<shared::networking::packets::ClientServerEntityUpdatePacket>(
                shared::networking::PacketFactory::CreatePacket(
                        shared::networking::PacketTypes::ClientServerEntityUpdate));

        clientServerEntityUpdatePacket->SetPlayerId(this->GetGame()->GetPlayer().GetPlayerId());
        clientServerEntityUpdatePacket->SetEntityId(entity->GetEntityId());
        clientServerEntityUpdatePacket->SetTimeOfUpdate(currentTime);
        clientServerEntityUpdatePacket->SetEntityType(entity->GetEntityType());

        auto data = entity->GetEntityData();

        clientServerEntityUpdatePacket->SetEntityData(data);

        this->SendPacketToServer(clientServerEntityUpdatePacket);

        entity->ResetBroadcastCounter();
    }

    void SceneManager::ReconfirmPixelSizes() noexcept
    {
        if (!this->_currentScene)
        {
            return;
        }

        this->_currentScene->ReconfirmPixelSizes();
    }
}