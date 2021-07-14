#include "graphics/shapes/shapes.h"
#include "render_manager.h"
#include "api/logging/logging.h"

namespace projectfarm::graphics
{
    const uint16_t RenderManager::RenderLayerId_Base = 0;

    bool RenderManager::Load()
    {
        shared::api::logging::Log("Loading render manager...");

        if (!this->AddRenderLayer(RenderManager::RenderLayerId_Base))
        {
            shared::api::logging::Log("Failed to add base render layer.");
            return false;
        }

        shared::api::logging::Log("Loaded render manager.");

        return true;
    }

    bool RenderManager::AddRenderLayer(uint16_t layerId)
    {
        if (this->GetRenderLayerByLayerId(layerId) != nullptr)
        {
            return false;
        }

        auto layer = std::make_shared<graphics::RenderLayer>(layerId);
        this->_renderLayers.emplace_back(std::move(layer));

        return true;
    }

    bool RenderManager::AddRenderable(uint16_t layerId, const std::shared_ptr<graphics::Renderable>& renderable)
    {
        auto layer = this->GetRenderLayerByLayerId(layerId);
        if (layer == nullptr)
        {
            return false;
        }

        layer->AddRenderable(renderable);

        return true;
    }

#ifdef DEBUG
    bool RenderManager::AddRenderableToDebugLayer(const std::shared_ptr<graphics::Renderable>& renderable)
    {
        if (this->_debugLayer == nullptr)
        {
            uint16_t id = 0;
            this->_debugLayer = std::make_shared<graphics::RenderLayer>(id);
        }

        this->_debugLayer->AddRenderable(renderable);

        return true;
    }

#endif

    void RenderManager::Shutdown()
    {
        shared::api::logging::Log("Shutting down render manager...");

        this->_renderLayers.clear();
        this->_uis.clear();

        shared::api::logging::Log("Shut down render manager.");
    }

    void RenderManager::Render()
    {
        for (const auto& layer : this->_renderLayers)
        {
            layer->Render();
            layer->Clear();
        }

        for (const auto& ui : this->_uis)
        {
            ui->Render();
        }

        this->_uis.clear();

#ifdef DEBUG
        if (this->_debugLayer != nullptr)
        {
            this->_debugLayer->Render();
            this->_debugLayer->Clear();
        }
#endif
    }
}