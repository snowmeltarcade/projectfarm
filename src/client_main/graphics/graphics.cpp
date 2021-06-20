#include <string>
#include <sstream>

#include <SDL.h>
#include <SDL_image.h>

#include "graphics_dependencies.h"
#include "graphics.h"
#include "engine/debug_information.h"
#include "opengl_errors.h"
#include "engine/game.h"

namespace projectfarm::graphics
{
	bool Graphics::Initialize(uint32_t screenWidthInMeters)
	{
		this->LogMessage("Initializing graphics...");

		constexpr static int sdlImageFlags = IMG_INIT_PNG;
		if (IMG_Init(sdlImageFlags) != sdlImageFlags)
		{
			this->LogMessage("Failed to initialize SDL Img.");
			this->LogMessage(SDL_GetError());
			return false;
		}
        
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
        
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

        this->_camera->SetLogger(this->_logger);
        this->_camera->SetGraphics(this->shared_from_this());
        this->_camera->SetDebugInformation(this->GetDebugInformation());
        this->_camera->SetScreenWidthInMeters(screenWidthInMeters);

        auto currentResolution = this->_camera->GetCurrentResolution();
        if (!currentResolution)
        {
            this->LogMessage("Failed to get current screen resolution.");
            return false;
        }

        auto windowX = currentResolution->FullScreen ? SDL_WINDOWPOS_CENTERED : SDL_WINDOWPOS_UNDEFINED;
        auto windowY = currentResolution->FullScreen ? SDL_WINDOWPOS_CENTERED : SDL_WINDOWPOS_UNDEFINED;
        auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                     SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN;

        if (currentResolution->FullScreen)
        {
            flags |= SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS;
        }

        this->_window = SDL_CreateWindow("Project Farm",
                                         windowX, windowY,
                                         currentResolution->Width, currentResolution->Height,
                                         flags);
		if (!this->_window)
		{
			this->LogMessage("Failed to create window.");
			this->LogMessage(SDL_GetError());
			return false;
		}

#if defined(USE_OPENGL_ES)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

		this->_context = SDL_GL_CreateContext(this->_window);
		if (!this->_context)
        {
		    this->LogMessage("Failed to create OpenGL context.");
		    this->LogMessage(SDL_GetError());
		    return false;
        }

#if !defined(USE_OPENGL_ES)
        glewExperimental = GL_TRUE;
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
        {
		    this->LogMessage("Failed to initialize GLEW.");
		    std::stringstream errorString;
            errorString << glewGetErrorString(glewError);
		    this->LogMessage(errorString.str());
		    return false;
        }
        CHECK_OPENGL_ERROR
#endif

		glClearColor(255, 0, 0, 1);
        CHECK_OPENGL_ERROR

#if !defined(USE_OPENGL_ES)
		// disable vsync if possible
		SDL_GL_SetSwapInterval(0);
        CHECK_OPENGL_ERROR
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        CHECK_OPENGL_ERROR

        this->_texturePool->SetLogger(this->_logger);
        this->_texturePool->SetDebugInformation(this->GetDebugInformation());
        this->_texturePool->SetGraphics(this->shared_from_this());

        this->_tileSetPool->SetLogger(this->_logger);
        this->_tileSetPool->SetDataProvider(this->_dataProvider);
        this->_tileSetPool->SetGraphics(this->shared_from_this());

		this->_renderManager->SetLogger(this->_logger);
        this->_renderManager->SetGraphics(this->shared_from_this());
		if (!this->_renderManager->Load())
        {
		    this->LogMessage("Failed to load render manager.");
		    return false;
        }

		if (!this->_camera->SetResolution(*currentResolution))
        {
		    this->LogMessage("Failed to set camera resolution.");
		    return false;
        }

		this->LogMessage("Initialized graphics.");

        for (const auto& [name, path] : this->_dataProvider->GetGraphicsShaderLocations())
        {
            auto shader = std::make_shared<Shader>();
            shader->SetLogger(this->_logger);
            shader->SetDataProvider(this->_dataProvider);

            if (!shader->Load(name))
            {
                this->LogMessage("Failed to load shader with name: " + name);
                return false;
            }

            if (shader->GetShaderType() == ShaderTypes::Vertex)
            {
                this->_vertexShaders[name] = shader;
            }
            else
            {
                this->_fragmentShaders[name] = shader;
            }
        }

        for (const auto& [name, path] : this->_dataProvider->GetGraphicsMaterialLocations())
        {
            auto material = std::make_shared<Material>();
            material->SetLogger(this->_logger);
            material->SetDataProvider(this->_dataProvider);
            material->SetGraphics(this->shared_from_this());

            if (!material->LoadFromFile(path))
            {
                this->LogMessage("Failed to load material with name: " + name);
                return false;
            }

            this->_materials[name] = material;
        }

        this->_mesh.SetLogger(this->_logger);
        this->_mesh.SetGraphics(this->shared_from_this());
        if (!this->_mesh.Load())
        {
            this->LogMessage("Failed to load mesh.");
        }

        this->_shapeMesh.SetLogger(this->_logger);
        this->_shapeMesh.SetGraphics(this->shared_from_this());
        if (!this->_shapeMesh.Load())
        {
            this->LogMessage("Failed to load shape mesh.");
        }

		return true;
	}

	void Graphics::Shutdown()
	{
		this->LogMessage("Shutting down graphics...");

		for (auto& material : this->_materials)
		{
			material.second->Destroy();
		}
		
		this->_mesh.Destroy();
        this->_shapeMesh.Destroy();

		this->_tileSetPool->Empty();
		this->_texturePool->Empty();

        if (this->_renderManager)
        {
            this->_renderManager->Shutdown();
            this->_renderManager = nullptr;
        }

        if (this->_context)
        {
            SDL_GL_DeleteContext(this->_context);
            this->_context = nullptr;
        }

        if (this->_window)
        {
            SDL_DestroyWindow(this->_window);
            this->_window = nullptr;
        }

		IMG_Quit();

		this->LogMessage("Shut down graphics.");
	}

	void Graphics::Render()
	{
	    this->BeginRender();

	    this->_renderManager->Render();

        auto numberOfMeshesRendered = this->_mesh.Render();
        auto numberOfShapesRendered = this->_shapeMesh.Render();

        this->GetDebugInformation()->AddDrawCall(numberOfMeshesRendered + numberOfShapesRendered);

	    this->EndRender();
	}

    void Graphics::BindMaterial(const std::string& name) noexcept
    {
        if (name == this->_currentlyBoundMaterialName)
        {
            return;
        }

        this->_currentlyBoundMaterialName = name;

        auto material = this->_materials.find(name);
        if (material == this->_materials.end())
        {
            this->LogMessage("Failed to find material with name: " + name);
            return;
        }

        if (this->_currentlyBoundMaterial)
        {
            this->_currentlyBoundMaterial->Unbind();
        }

        this->_currentlyBoundMaterial = material->second;

        this->_currentlyBoundMaterial->Bind();
    }

    std::shared_ptr<SDLFreeableSurface> Graphics::CreateSurface(const shared::graphics::colors::Color& color,
                                                                uint32_t width, uint32_t height) const noexcept
    {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        uint32_t rMask = 0xff000000;
        uint32_t gMask = 0x00ff0000;
        uint32_t bMask = 0x0000ff00;
        uint32_t aMask = 0x000000ff;
#else
        uint32_t rMask = 0x000000ff;
        uint32_t gMask = 0x0000ff00;
        uint32_t bMask = 0x00ff0000;
        uint32_t aMask = 0xff000000;
#endif

        auto surface = SDL_CreateRGBSurface(0,
                                            width, height,
                                            32, rMask, gMask, bMask, aMask);

        SDL_FillRect(surface, nullptr,
                     SDL_MapRGBA(surface->format,
                                 color.r, color.g, color.b, color.a));

        return std::make_shared<SDLFreeableSurface>(surface);
    }

    void Graphics::StampSurface(SDL_Surface* surface, const shapes::Rectangle& rect) const noexcept
    {
        SDL_Rect renderRect
        {
            rect.GetX(),
            rect.GetY(),
            static_cast<int>(rect.GetW()),
            static_cast<int>(rect.GetH()),
        };

        auto rectColor = rect.GetColor();
        auto color = SDL_MapRGBA(surface->format,
                                 rectColor.r, rectColor.g, rectColor.b, rectColor.a);

        if (renderRect.w == 0 || renderRect.h == 0)
        {
            SDL_FillRect(surface, nullptr, color);
        }
        else
        {
            SDL_FillRect(surface, &renderRect, color);
        }
    }

    void Graphics::StampSurface(SDL_Surface* surface, const shared::graphics::colors::Color& c) const noexcept
    {
        auto color = SDL_MapRGBA(surface->format,
                                 c.r, c.g, c.b, c.a);

        SDL_FillRect(surface, nullptr, color);
    }

	void Graphics::BeginRender()
	{
	    glClear(GL_COLOR_BUFFER_BIT);
	}

    bool Graphics::EndRender()
    {
	    SDL_GL_SwapWindow(this->_window);

	    this->_mesh.ClearTileData();
        this->_shapeMesh.ClearShapeData();

        CHECK_OPENGL_ERROR

        return true;
    }

    void Graphics::PushTileToRender(const std::shared_ptr<Texture>& texture,
                                    SDL_Rect* source, SDL_Rect* dest,
                                    uint32_t renderLayerIndex)
    {
	    int dx = 0;
	    int dy = 0;
	    int dw = this->_camera->GetViewport().w;
	    int dh = this->_camera->GetViewport().h;

        int sx = 0;
        int sy = 0;
        int sw = 0;
        int sh = 0;

	    if (dest)
	    {
	        dx = dest->x;
            dy = dest->y;
            dw = dest->w;
            dh = dest->h;
	    }

	    if (source)
        {
            sx = source->x;
            sy = source->y;
            sw = source->w;
            sh = source->h;
        }

	    this->_mesh.AddTileData(texture,
                                static_cast<float>(sx), static_cast<float>(sy),
                                static_cast<float>(sw), static_cast<float>(sh),
	                            static_cast<float>(dx), static_cast<float>(dy),
	                            static_cast<float>(dw), static_cast<float>(dh),
                                this->_camera->GetViewport().w, this->_camera->GetViewport().h,
	                            renderLayerIndex);
    }

    void Graphics::PushShapeToRender(const shapes::Rectangle& shape,
                                     const SDL_Rect& screenSpace,
                                     uint32_t renderLayerIndex)
    {
        this->_shapeMesh.AddShapeData(shape,
                                      screenSpace,
                                      this->_camera->GetViewport().w, this->_camera->GetViewport().h,
                                      renderLayerIndex);
    }

    void Graphics::EndFrame()
    {
	    using namespace std::chrono_literals;

	    this->_currentRenderLayer = 0;

	    this->_texturePool->Cleanup(10min);
    }

    void Graphics::SceneLoaded()
    {
	    this->_tileSetPool->Empty();
    }
    
    void Graphics::OnWindowResized(uint32_t width, uint32_t height) noexcept
    {
	    auto currentResolution = this->_camera->GetCurrentResolution();
	    if (!currentResolution)
        {
	        this->LogMessage("Failed to get current screen resolution.");
	        return;
        }

	    currentResolution->Width = width;
        currentResolution->Height = height;

        if (!this->_camera->SetResolution(*currentResolution))
        {
            this->LogMessage("Failed to set screen resolution to: " + currentResolution->GetName());
            return;
        }

        this->GetGame()->ReconfirmPixelSizes();
    }
}
