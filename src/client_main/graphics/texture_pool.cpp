#include <algorithm>
#include <list>

#include "texture_pool.h"
#include "engine/debug_information.h"
#include "graphics/opengl_errors.h"
#include "api/logging/logging.h"

namespace projectfarm::graphics
{
    TexturePoolData TexturePool::Get(const std::string& name)
    {
        if (this->_textures.count(name) > 0)
        {
            this->_textures[name].ReferenceCount++;
            return this->_textures[name];
        }

        auto data = this->Create(name);
        if (data.TextureId == 0)
        {
            shared::api::logging::Log("Failed to create texture with name: " + name);
            return {};
        }

        this->_textures[name] = data;
        this->_textures[name].ReferenceCount++;

        return this->_textures[name];
    }

    TexturePoolData TexturePool::Create(const std::filesystem::path& path)
    {
        TexturePoolData data;

        auto imagePath = path.u8string();
        SDL_Surface* surface = IMG_Load(imagePath.c_str());
        if (surface == nullptr)
        {
            this->_logger->LogMessage("Failed to load the texture:");
            this->_logger->LogMessage(imagePath);
            this->_logger->LogMessage(IMG_GetError());
            return data;
        }

        data = this->CreateTexture(surface);

        SDL_FreeSurface(surface);
        surface = nullptr;

        if (data.TextureId == 0)
        {
            shared::api::logging::Log("Failed to create texture.");
        }
        else
        {
            this->GetDebugInformation()->AddNumberOfTexturesLoaded(1);
        }

        return data;
    }

    void TexturePool::Release(GLuint textureId)
    {
        auto iter = std::find_if(this->_textures.begin(), this->_textures.end(), [textureId](const auto& it)
        {
           return it.second.TextureId == textureId;
        });

        if (iter == this->_textures.end())
        {
            return;
        }

        (*iter).second.ReferenceCount--;

        if ((*iter).second.ReferenceCount <= 0)
        {
            this->Destroy(textureId);
        }
    }

    void TexturePool::Destroy(GLuint textureId, bool removeFromMap)
    {
        auto iter = std::find_if(this->_textures.begin(), this->_textures.end(), [textureId](const auto& it)
        {
            return it.second.TextureId == textureId;
        });

        if (iter == this->_textures.end())
        {
            return;
        }

        glDeleteTextures(1, &textureId);

        if (removeFromMap)
        {
            this->_textures.erase((*iter).first);
        }
    }

    void TexturePool::Cleanup(const std::chrono::duration<uint64_t, std::milli>& when)
    {
        auto now = std::chrono::steady_clock::now();

        if (now >= this->_nextCleanupTime)
        {
            this->_lastCleanupTime = this->_nextCleanupTime;
            this->_nextCleanupTime = now + when;

            this->PerformCleanup();
        }
    }

    void TexturePool::Empty()
    {
        for (const auto&[_, texture] : this->_textures)
        {
            this->Destroy(texture.TextureId, false);
        }

        this->_textures.clear();
    }

    TexturePoolData TexturePool::CreateTexture(SDL_Surface* surface)
    {
        TexturePoolData data;

        data.Width = static_cast<uint32_t>(surface->w);
        data.Height = static_cast<uint32_t>(surface->h);

        if (!this->GenerateTexture(data, surface))
        {
            shared::api::logging::Log("Failed to generate texture.");
            return data;
        }

        return data;
    }

    bool TexturePool::GenerateTexture(TexturePoolData& data, SDL_Surface* surface)
    {
        glGenTextures(1, &data.TextureId);
        glBindTexture(GL_TEXTURE_2D, data.TextureId);
        CHECK_OPENGL_ERROR

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        CHECK_OPENGL_ERROR

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        CHECK_OPENGL_ERROR

        GLenum format = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
        uint32_t bitDepth = surface->format->BitsPerPixel;

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

        SDL_Surface* newSurface = SDL_CreateRGBSurface(0, data.Width, data.Height,
                                                       bitDepth, rMask, gMask, bMask, aMask);

        SDL_BlitSurface(surface, nullptr, newSurface, nullptr);
        CHECK_OPENGL_ERROR

        glTexImage2D(GL_TEXTURE_2D, 0, format,
                     data.Width, data.Height,
                     0, format, GL_UNSIGNED_BYTE, newSurface->pixels);

        SDL_FreeSurface(newSurface);
        newSurface = nullptr;

        CHECK_OPENGL_ERROR

        glGenerateMipmap(GL_TEXTURE_2D);
        CHECK_OPENGL_ERROR

        return true;
    }

    void TexturePool::PerformCleanup()
    {
        shared::api::logging::Log("Performing texture pool cleanup...");

        std::list<std::map<std::string, TexturePoolData>::iterator> itemsToRemove;

        for (auto it = this->_textures.begin(); it != this->_textures.end(); it++)
        {
            if (it->second.ReferenceCount == 0)
            {
                glDeleteTextures(1, &it->second.TextureId);

                itemsToRemove.emplace_back(it);
            }
        }

        for (auto& it : itemsToRemove)
        {
            this->_textures.erase(it);
        }

        shared::api::logging::Log("Removed " + std::to_string(itemsToRemove.size()) + " texture(s) from the texture pool.");
    }
}
