#ifndef PROJECTFARM_MATERIAL_H
#define PROJECTFARM_MATERIAL_H

#include <filesystem>

#include "logging/consume_logger.h"
#include "data/consume_data_provider.h"
#include "graphics/colors/color.h"
#include "shader_program.h"
#include "consume_graphics.h"

namespace projectfarm::graphics
{
    class Material : public shared::ConsumeLogger,
                     public projectfarm::shared::ConsumeDataProvider,
                     public ConsumeGraphics
    {
    public:
        Material() = default;
        ~Material() override = default;

        [[nodiscard]]
        bool LoadFromFile(const std::filesystem::path& path) noexcept;

        void Destroy() noexcept;

        void Bind() const noexcept
        {
            this->_shaderProgram.Use();
        }

        void Unbind() const noexcept
        {
            this->_shaderProgram.ClearUse();
        }

        void SetColor(const shared::graphics::colors::Color& color) noexcept;

    private:
        ShaderProgram _shaderProgram;
        shared::graphics::colors::Color _color {shared::graphics::colors::White};
        uint32_t _numberOfTextures {0};

        [[nodiscard]]
        bool LoadShader(const std::string& vertexShaderName, const std::string& fragmentShaderName) noexcept;
    };
}

#endif
