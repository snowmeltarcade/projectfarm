#ifndef PROJECTFARM_TEXTURE_POOL_H
#define PROJECTFARM_TEXTURE_POOL_H

#include <map>
#include <string>
#include <filesystem>
#include <chrono>

#include <SDL.h>
#include <SDL_image.h>

#include "logging/consume_logger.h"
#include "consume_graphics.h"
#include "engine/consume_debug_information.h"
#include "texture_pool_data.h"
#include "graphics_dependencies.h"

namespace projectfarm::graphics
{
    class TexturePool final : public shared::ConsumeLogger,
                              public ConsumeGraphics,
                              public engine::ConsumeDebugInformation
    {
    public:
        TexturePool()
        {
            this->_lastCleanupTime = std::chrono::steady_clock::now();
            this->_nextCleanupTime = this->_lastCleanupTime;
        }
        ~TexturePool() override = default;

        [[nodiscard]] TexturePoolData Get(const std::filesystem::path& name)
        {
            return this->Get(name.u8string());
        }

        [[nodiscard]] TexturePoolData Get(const std::string& name);

        [[nodiscard]] TexturePoolData Create(const std::filesystem::path& path);

        void Release(GLuint textureId);
        void Destroy(GLuint textureId, bool removeFromMap = true);

        void Cleanup(const std::chrono::duration<uint64_t, std::milli>& when);

        void Empty();

        [[nodiscard]] TexturePoolData CreateTexture(SDL_Surface* surface);

    private:
        [[nodiscard]] bool GenerateTexture(TexturePoolData& data, SDL_Surface* surface);

        void PerformCleanup();

        std::map<std::string, TexturePoolData> _textures;

        std::chrono::time_point<std::chrono::steady_clock> _lastCleanupTime;
        std::chrono::time_point<std::chrono::steady_clock> _nextCleanupTime;
    };
}

#endif
