#ifndef PROJECTFARM_SHADER_PROGRAM_H
#define PROJECTFARM_SHADER_PROGRAM_H

#include <string>
#include <array>

#include "graphics_dependencies.h"
#include "shader.h"

namespace projectfarm::graphics
{
    class ShaderProgram final
    {
    public:
        ShaderProgram() = default;
        ~ShaderProgram() = default;

        [[nodiscard]]
        bool Create() noexcept;

        void Destroy() noexcept;

        [[nodiscard]] bool AttachShader(const std::shared_ptr<Shader>& shader) noexcept;
        [[nodiscard]] bool Link() noexcept;

        [[nodiscard]] GLuint GetProgramId() const noexcept
        {
            return this->_programId;
        }

        void Use() const noexcept
        {
            glUseProgram(this->_programId);
        }

        void ClearUse() const noexcept
        {
            glUseProgram(0);
        }

        void SetUniformVec4(const std::string& name, std::array<float, 4> data) const noexcept;

    private:
        GLuint _programId {0};
        bool _isLinked {false};

        void LogErrors() noexcept;
    };
}

#endif
