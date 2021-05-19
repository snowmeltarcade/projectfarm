#ifndef LOADING_SCENE_H
#define LOADING_SCENE_H

#include "../scene.h"
#include "graphics/ui/ui.h"
#include "graphics/shader_program.h"

#include <memory>

namespace projectfarm::scenes::implemented_scenes
{
	class LoadingScene final : public Scene
	{
	public:
		LoadingScene() = default;
		~LoadingScene() override = default;

//		[[nodiscard]] projectfarm::scenes::SceneTypes GetSceneType() const override
//		{
//			return projectfarm::scenes::SceneTypes::Loading;
//		}

		[[nodiscard]] bool ShouldQuit() const override
		{
			return this->_shouldQuit;
		}

		void ProcessPacket(const std::shared_ptr<shared::networking::Packet>& packet) override;

		void UpdateInput() override;

		bool Initialize() override;

		void PrepareRender() override;

		void Tick() override;

		void Destroy() override;

		void Quit() override;

        void OnPlayerCharacterAdd(const std::shared_ptr<engine::entities::Character>&) noexcept override
        {
        }

        void ReconfirmPixelSizes() noexcept override
        {
        }

	private:
		bool _shouldQuit {false};

		bool _isLoadingNewScene {false};
		std::string _worldToLoad;

        std::shared_ptr<projectfarm::graphics::ui::UI> _ui;

		[[nodiscard]]
        bool ValidatePacket(const std::shared_ptr<shared::networking::Packet>& packet) const;

		void HandleServerClientLoadWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet);

		[[nodiscard]]
        bool SetupUI();

		void LoadNewWorld(const std::string& worldName) noexcept;
	};
}

#endif
