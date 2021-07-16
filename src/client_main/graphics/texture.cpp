#include "texture.h"
#include "graphics.h"
#include "opengl_errors.h"
#include "api/logging/logging.h"

namespace projectfarm::graphics
{
	bool Texture::Load(const std::filesystem::path& path) noexcept
	{
	    auto data = this->GetGraphics()->GetTexturePool()->Get(path);
        if (data.TextureId == 0)
        {
            shared::api::logging::Log("Failed to create texture.");
            return false;
        }

        this->_textureId = data.TextureId;
        this->_textureWidth = data.Width;
        this->_textureHeight = data.Height;

        this->_loadedDirect = false;

        return true;
	}

    bool Texture::LoadFromSurface(const std::shared_ptr<SDLFreeableSurface>& surface) noexcept
    {
	    this->Destroy();

	    auto data = this->GetGraphics()->GetTexturePool()->CreateTexture(surface->GetSurface());

        if (data.TextureId == 0)
        {
            shared::api::logging::Log("Failed to create texture.");
            return false;
        }

        this->_textureId = data.TextureId;
        this->_textureWidth = data.Width;
        this->_textureHeight = data.Height;

        this->_loadedDirect = true;

        return true;
    }

	bool Texture::Destroy()
	{
        if (this->_textureId <= 0)
        {
            return true;
        }

	    if (this->_loadedDirect)
        {
	        glDeleteTextures(1, &this->_textureId);
        }
	    else
        {
	        this->GetGraphics()->GetTexturePool()->Release(this->_textureId);
	    }

	    this->_textureId = 0;

	    this->_loadedDirect = false;

	    CHECK_OPENGL_ERROR

	    return true;
	}
	
	void Texture::Render(uint32_t renderLayerIndex)
	{
        // TODO: Setting the dest and source rect seems like it doesn't have to be done on each render call
	    SDL_Rect destRect;
	    destRect.x = this->_renderX;
	    destRect.y = this->_renderY;
	    destRect.w = this->_width;
	    destRect.h = this->_height;

	    // the default rendering origin point is top left. check for others...
	    if (this->_renderOriginPoint == RenderOriginPoints::Center)
        {
	        destRect.x -= static_cast<int>(this->_width / 2);
	        destRect.y -= static_cast<int>(this->_height / 2);
        }

	    SDL_Rect sourceRect;
	    if (this->_originWidth > 0 || this->_originHeight > 0)
        {
	        sourceRect.x = this->_originX;
            sourceRect.y = this->_originY;
            sourceRect.w = this->_originWidth;
            sourceRect.h = this->_originHeight;
        }
	    else
        {
            sourceRect.x = 0;
            sourceRect.y = 0;
            sourceRect.w = this->_textureWidth;
            sourceRect.h = this->_textureHeight;
        }

		if (!this->GetGraphics()->GetCamera()->RenderTexture(this->shared_from_this(),
		                                                     &sourceRect, &destRect,
		                                                     this->_renderToWorldSpace,
		                                                     renderLayerIndex))
		{
			shared::api::logging::Log("Failed to render texture.");
			shared::api::logging::Log(SDL_GetError());
			return;
		}
	}

    void Texture::Bind() const noexcept
    {
        // set this here in case there are no main textures, but
        // we have material textures
        auto textureIndex {0u};

        // this could be a color, so no need to bind an actual texture
        if (this->_textureId != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->_textureId);

            ++textureIndex;
        }

        for (const auto& texture : this->_materialTextures)
        {
            glActiveTexture(GL_TEXTURE0 + textureIndex);
            ++textureIndex;

            glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
        }
    }
}