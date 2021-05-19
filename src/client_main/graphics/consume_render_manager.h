#ifndef CONSUME_RENDER_MANAGER_H
#define CONSUME_RENDER_MANAGER_H

#include <memory>

namespace projectfarm::graphics
{
	class RenderManager;

	class ConsumeRenderManager
	{
	public:
        ConsumeRenderManager() = default;
		virtual ~ConsumeRenderManager() = default;

        [[nodiscard]] const std::shared_ptr<RenderManager>& GetRenderManager() const
		{
			return _renderManager;
		}

		void SetRenderManager(const std::shared_ptr<RenderManager>& renderManager)
		{
			this->_renderManager = renderManager;
		}

	protected:
		std::shared_ptr<RenderManager> _renderManager;
	};
}

#endif