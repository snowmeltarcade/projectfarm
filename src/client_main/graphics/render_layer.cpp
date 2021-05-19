#include "render_layer.h"

namespace projectfarm::graphics
{
    void RenderLayer::Render()
    {
        for (const auto& renderable : this->_renderables)
        {
            renderable->Render();
        }
    }
}