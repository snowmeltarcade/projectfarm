#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <memory>
#include <atomic>
#include <optional>
#include <functional>
#include <unordered_map>

#include "engine/consume_debug_information.h"
#include "engine/consume_game.h"
#include "network_client/consume_network_client.h"
#include "networking/packet.h"
#include "time/consume_timer.h"
#include "engine/consume_debug_information.h"
#include "scene.h"
#include "scripting/consume_script_system.h"

namespace projectfarm::scenes
{
	class SceneManager final : public engine::ConsumeGame,
						       public network_client::ConsumeNetworkClient,
                               public shared::time::ConsumeTimer,
                               public engine::ConsumeDebugInformation,
                               public shared::scripting::ConsumeScriptSystem,
                               public std::enable_shared_from_this<SceneManager>
	{
	public:
		SceneManager() = default;
		~SceneManager() override = default;

		SceneManager(const SceneManager&) = delete;
		SceneManager(SceneManager&&) = delete;

		void AddLoadSceneParameter(const std::string& key, const std::string& value)
        {
		    this->_sceneLoadParameters[key] = value;
        }

		void QueueLoadScene(SceneTypes type);
		[[nodiscard]] bool HandleQueuedScene(const std::function<void(void)>& onSceneLoadHandler);

		[[nodiscard]] bool LoadScene(SceneTypes type);

        [[nodiscard]] const std::shared_ptr<Scene>& GetCurrentScene() const
		{
			return this->_currentScene;
		}

        [[nodiscard]] std::shared_ptr<Scene> CreateSceneBySceneType(SceneTypes type) const;

		void PassPacketToCurrentScene(const std::shared_ptr<shared::networking::Packet>& packet);

		void Tick();

		void SendPacketToServer(const std::shared_ptr<shared::networking::Packet>& packet) const noexcept;

        void BroadcastEntityState(const std::shared_ptr<engine::entities::Entity>& entity, uint64_t currentTime) const noexcept;

        void ReconfirmPixelSizes() noexcept;

	private:
		std::shared_ptr<Scene> _currentScene;

		// used to pass network packets from the network manager to the current scene
		std::mutex _networkPacketMutex;

		std::atomic<bool> _hasNetworkPacketsToProcess = false;
		std::vector<std::shared_ptr<shared::networking::Packet>> _networkPacketsToSendToCurrentScene;

		std::optional<projectfarm::scenes::SceneTypes> _queuedSceneType;

		std::unordered_map<std::string, std::string> _sceneLoadParameters;

		void HandleNetworkPackets();
	};
}

#endif