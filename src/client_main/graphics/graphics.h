#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <SDL.h>

#include "camera.h"
#include "render_manager.h"
#include "logging/consume_logger.h"
#include "ui/ui.h"
#include "engine/consume_debug_information.h"
#include "tiling_mesh.h"
#include "shape_mesh.h"
#include "material.h"
#include "texture_pool.h"
#include "tile_set_pool.h"
#include "data/consume_data_provider.h"
#include "engine/consume_game.h"
#include "shapes/shapes.h"

namespace projectfarm::graphics
{
	class Graphics final : public projectfarm::shared::ConsumeLogger,
	                       public engine::ConsumeDebugInformation,
	                       public shared::ConsumeDataProvider,
	                       public engine::ConsumeGame,
	                       public std::enable_shared_from_this<Graphics>
	{
	public:
		Graphics()
        {
		    this->_camera = std::make_shared<Camera>();
            this->_renderManager = std::make_shared<RenderManager>();
            this->_texturePool = std::make_shared<TexturePool>();
            this->_tileSetPool = std::make_shared<TileSetPool>();
        }
		~Graphics() override = default;

		[[nodiscard]]
		bool Initialize(uint32_t screenWidth, uint32_t screenHeight,
                        bool fullScreen, uint32_t screenWidthInMeters);

		void Shutdown();

		[[nodiscard]]
		SDL_Renderer* GetRenderer() const noexcept
		{
			return this->_renderer;
		}

		[[nodiscard]]
		const std::shared_ptr<RenderManager>& GetRenderManager() const noexcept
        {
		    return this->_renderManager;
        }

        [[nodiscard]]
        SDL_Window* GetWindow() const noexcept
        {
		    return this->_window;
        }

        [[nodiscard]]
        uint32_t GetScreenWidth() const noexcept
        {
            return this->_screenWidth;
        }

        [[nodiscard]]
        uint32_t GetScreenHeight() const noexcept
        {
            return this->_screenHeight;
        }

        [[nodiscard]]
        const std::shared_ptr<Camera>& GetCamera() const noexcept
        {
		    return this->_camera;
        }

        [[nodiscard]]
        const std::shared_ptr<TexturePool>& GetTexturePool() const noexcept
        {
		    return this->_texturePool;
        }

        [[nodiscard]]
        const std::shared_ptr<TileSetPool>& GetTileSetPool() const noexcept
        {
            return this->_tileSetPool;
        }

		void Render();

		void PushTileToRender(const std::shared_ptr<Texture>& texture,
		                      SDL_Rect* source, SDL_Rect* dest,
		                      uint32_t renderLayerIndex);

        void PushShapeToRender(const shapes::Rectangle& shape,
                               const SDL_Rect& screenSpace,
                               uint32_t renderLayerIndex);

		void EndFrame();

        void SceneLoaded();

        [[nodiscard]]
        uint32_t BumpRenderLayer() noexcept
        {
            return ++this->_currentRenderLayer;
        }

        [[nodiscard]]
        uint32_t GetRenderLayer() const noexcept
        {
            return this->_currentRenderLayer;
        }

        void SetRenderLayer(uint32_t renderLayer) noexcept
        {
            this->_currentRenderLayer = renderLayer;
        }
        
        void OnWindowResized(uint32_t width, uint32_t height) noexcept;

        // this return value is intentionally a signed integer
        [[nodiscard]]
        int32_t MetersToPixels(float meters) const noexcept
        {
            return static_cast<int32_t>(this->GetCamera()->GetPixelsPerMeter() * meters);
        }

        [[nodiscard]]
        const std::shared_ptr<Shader>& GetVertexShaderByName(const std::string& name) noexcept
        {
            return this->_vertexShaders[name];
        }

        [[nodiscard]]
        const std::shared_ptr<Shader>& GetFragmentShaderByName(const std::string& name) noexcept
        {
            return this->_fragmentShaders[name];
        }

        void BindMaterial(const std::string& name) noexcept;

        [[nodiscard]]
        const std::shared_ptr<Material>& GetCurrentlyBoundMaterial() const noexcept
        {
            return this->_currentlyBoundMaterial;
        }

        [[nodiscard]]
        std::shared_ptr<SDLFreeableSurface> CreateSurface(const shared::graphics::colors::Color& color,
                                                          uint32_t width, uint32_t height) const noexcept;

        // this stamps the surface with the color and area defined by the rectangle
        void StampSurface(SDL_Surface* surface, const shapes::Rectangle& rect) const noexcept;

        // this stamps the entire surface with the passed color
        void StampSurface(SDL_Surface* surface, const shared::graphics::colors::Color& color) const noexcept;

	private:
        void BeginRender();
        bool EndRender();

		SDL_Window* _window {nullptr};
		SDL_Renderer* _renderer {nullptr};
		SDL_GLContext _context {nullptr};

        uint32_t _screenWidth {0};
        uint32_t _screenHeight {0};

		std::shared_ptr<Camera> _camera;
		std::shared_ptr<RenderManager> _renderManager;

		TilingMesh _mesh;
        ShapeMesh _shapeMesh;

        std::unordered_map<std::string, std::shared_ptr<Shader>> _vertexShaders;
        std::unordered_map<std::string, std::shared_ptr<Shader>> _fragmentShaders;

        std::string _currentlyBoundMaterialName;
        std::shared_ptr<Material> _currentlyBoundMaterial;
        std::unordered_map<std::string, std::shared_ptr<Material>> _materials;

        std::shared_ptr<TexturePool> _texturePool;
        std::shared_ptr<TileSetPool> _tileSetPool;

        uint32_t _currentRenderLayer {0};
	};
}

#endif
