#ifndef PROJECTFARM_RENDERER_H
#define PROJECTFARM_RENDERER_H

#include <vector>
#include <memory>
#include <algorithm>

#include "logging/consume_logger.h"
#include "ui/ui.h"
#include "render_layer.h"
#include "graphics/colors/color.h"

namespace projectfarm::graphics
{
    class RenderManager final : public shared::ConsumeLogger,
                                public ConsumeGraphics
    {
    public:
        RenderManager() = default;
        ~RenderManager() override = default;

        [[nodiscard]] bool Load();
        void Shutdown();

        [[nodiscard]] bool AddRenderLayer(uint16_t layerId);
        [[nodiscard]] bool AddRenderable(uint16_t layerId, const std::shared_ptr<graphics::Renderable>& renderable);

#ifdef DEBUG
        [[nodiscard]] bool AddRenderableToDebugLayer(const std::shared_ptr<graphics::Renderable>& renderable);
#endif

        void Render();

        void AddUI(const std::shared_ptr<graphics::ui::UI>& ui) noexcept
        {
            this->_uis.emplace_back(ui);
        }

        [[nodiscard]] std::shared_ptr<graphics::RenderLayer> GetRenderLayerByLayerId(uint16_t layerId)
        {
            auto it = std::find_if(this->_renderLayers.begin(), this->_renderLayers.end(),
                    [layerId](const auto& layer) -> bool { return layer->GetLayerId() == layerId; });

            return it == this->_renderLayers.end() ? nullptr : *it;
        }

        static const uint16_t RenderLayerId_Base;

    private:
        std::vector<std::shared_ptr<graphics::RenderLayer>> _renderLayers;

        std::vector<std::shared_ptr<graphics::ui::UI>> _uis;

#ifdef DEBUG
        std::shared_ptr<graphics::RenderLayer> _debugLayer;
#endif
    };
}

#endif
