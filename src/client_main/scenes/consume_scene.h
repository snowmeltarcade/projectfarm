#ifndef CONSUME_SCENE_H
#define CONSUME_SCENE_H

#include <memory>
#include <mutex>

namespace projectfarm::scenes
{
	class Scene;

	class ConsumeScene
	{
	public:
        ConsumeScene() = default;
		virtual ~ConsumeScene() = default;

        [[nodiscard]] const std::shared_ptr<Scene>& GetScene() const
		{
			return _scene;
		}

		void SetScene(const std::shared_ptr<Scene>& scene)
		{
			this->_scene = scene;
		}

	private:
		std::shared_ptr<Scene> _scene;
	};
}

#endif
