#ifndef PROJECTFARM_RENDER_LAYER_H
#define PROJECTFARM_RENDER_LAYER_H

#include <vector>
#include <memory>

#include "renderable.h"

namespace projectfarm::graphics
{
    class RenderLayer final
    {
    public:
        explicit RenderLayer(uint16_t layerId) : _layerId(layerId)
        {}
        ~RenderLayer() = default;

        void Render();

        void Clear()
        {
            this->_renderables.clear();
        }

        void AddRenderable(const std::shared_ptr<graphics::Renderable>& renderable)
        {
            this->_renderables.emplace_back(renderable);
        }

        [[nodiscard]] uint16_t GetLayerId() const
        {
            return this->_layerId;
        }

    private:
        uint16_t _layerId;

        std::vector<std::shared_ptr<graphics::Renderable>> _renderables;
    };
}

#endif
