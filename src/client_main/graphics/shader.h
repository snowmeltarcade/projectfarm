#ifndef PROJECTFARM_SHADER_H
#define PROJECTFARM_SHADER_H

#include <string>
#include <optional>
#include <algorithm>

#include "graphics_dependencies.h"
#include "data/consume_data_provider.h"
#include "shader_types.h"

namespace projectfarm::graphics
{
    class Shader final : public projectfarm::shared::ConsumeDataProvider
    {
    public:
        Shader() = default;
        ~Shader() override = default;

        [[nodiscard]]
        bool Load(const std::string& name) noexcept;

        void Destroy() noexcept;

        [[nodiscard]]
        GLuint GetShaderId() const noexcept
        {
            return this->_shaderId;
        }

        [[nodiscard]]
        ShaderTypes GetShaderType() const noexcept
        {
            return this->_type;
        }

    private:
        GLuint _shaderId {0};
        ShaderTypes _type {ShaderTypes::Vertex};

        void LogErrors() const noexcept;

        [[nodiscard]]
        std::optional<std::pair<std::string, ShaderTypes>> GetShaderCode(const std::string& name) const noexcept;
    };
}

#endif
