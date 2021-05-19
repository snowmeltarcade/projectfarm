#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <memory>
#include <SDL.h>

#include "graphics_dependencies.h"
#include "renderable.h"
#include "SDL_freeable_surface.h"
#include "graphics/colors/color.h"
#include "shapes/shapes.h"

namespace projectfarm::graphics
{
    class Texture final : public std::enable_shared_from_this<Texture>,
                          public Renderable
	{
	public:
		Texture() = default;
		~Texture() override
        {
            this->Destroy();
        }

		[[nodiscard]]
        bool Load(const std::filesystem::path& path) noexcept;

		[[nodiscard]]
        bool LoadFromSurface(const std::shared_ptr<SDLFreeableSurface>& surface) noexcept;

		bool Destroy();
		
		void Render() noexcept override
        {
		    this->Render(0);
        }

        void Render(uint32_t renderLayerIndex);

		void Bind() const noexcept;

        [[nodiscard]]
        GLuint GetTextureId() const noexcept
        {
		    return this->_textureId;
        }

        [[nodiscard]]
        uint32_t GetTextureWidth() const noexcept
        {
		    return this->_textureWidth;
        }

        void SetTextureWidth(uint32_t width) noexcept
        {
            this->_textureWidth = width;
        }

        [[nodiscard]]
        uint32_t GetTextureHeight() const noexcept
        {
            return this->_textureHeight;
        }

        void SetTextureHeight(uint32_t height) noexcept
        {
            this->_textureHeight = height;
        }

        [[nodiscard]]
        const std::string& GetMaterialName() const noexcept
        {
            return this->_materialName;
        }

        void SetMaterialName(const std::string& name) noexcept
        {
            this->_materialName = name;
        }

        void SetColor(const shared::graphics::colors::Color& color) noexcept
        {
            this->_color = color;
        }

        [[nodiscard]]
        const shared::graphics::colors::Color& GetColor() const noexcept
        {
            return this->_color;
        }

        void ClearMaterialTextures() noexcept
        {
            this->_materialTextures.clear();
        }

        void AddMaterialTexture(const std::shared_ptr<Texture>& texture) noexcept
        {
            this->_materialTextures.push_back(texture);
        }

	private:
		uint32_t _textureWidth {0};
		uint32_t _textureHeight {0};

		GLuint _textureId {0};

		// this will be true if this texture was loaded directly from an SDL_Surface and
		// not from the texture pool, so we will need to manually delete the texture
		bool _loadedDirect {false};

        std::string _materialName = "single_texture";

        shared::graphics::colors::Color _color {shared::graphics::colors::White};

        std::vector<std::shared_ptr<Texture>> _materialTextures;
	};
}

#endif
