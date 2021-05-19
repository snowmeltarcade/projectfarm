#ifndef CONSUME_SCENE_MANAGER_H
#define CONSUME_SCENE_MANAGER_H

#include <memory>
#include <mutex>

namespace projectfarm::scenes
{
	class SceneManager;

	class ConsumeSceneManager
	{
	public:
		ConsumeSceneManager() = default;
		virtual ~ConsumeSceneManager() = default;

        [[nodiscard]] const std::shared_ptr<SceneManager>& GetSceneManager() const
		{
			return _sceneManager;
		}

		void SetSceneManager(const std::shared_ptr<SceneManager>& sceneManager)
		{
			this->_sceneManager = sceneManager;
		}

	private:
		std::shared_ptr<SceneManager> _sceneManager;
	};
}

#endif
