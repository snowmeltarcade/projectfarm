#include "shader_program.h"

namespace projectfarm::graphics
{
    bool ShaderProgram::Create() noexcept
    {
        if (this->_programId != 0)
        {
            this->Destroy();
        }

        this->_programId = glCreateProgram();

        return true;
    }

    void ShaderProgram::Destroy() noexcept
    {
        if (this->_programId != 0)
        {
            glDeleteProgram(this->_programId);
            this->_programId = 0;
        }

        this->_isLinked = false;
    }

    bool ShaderProgram::AttachShader(const std::shared_ptr<Shader>& shader) noexcept
    {
        if (this->_isLinked)
        {
            this->LogMessage("Cannot attach shader after shader program has linked.");
            return false;
        }

        glAttachShader(this->_programId, shader->GetShaderId());

        return true;
    }

    bool ShaderProgram::Link() noexcept
    {
        if (this->_isLinked)
        {
            this->LogMessage("Shader program already linked.");
            return false;
        }

        glLinkProgram(this->_programId);

        GLint linkSuccess = GL_FALSE;
        glGetProgramiv(this->_programId, GL_LINK_STATUS, &linkSuccess);
        if (linkSuccess != GL_TRUE)
        {
            this->LogMessage("Failed to link shader program.");
            this->LogErrors();
            return false;
        }

        this->_isLinked = true;

        return true;
    }

    void ShaderProgram::LogErrors() noexcept
    {
        GLint logLength = 0;
        GLint actualLogLength = 0;

        glGetProgramiv(this->_programId, GL_INFO_LOG_LENGTH, &logLength);

        char* log = new char[logLength];

        glGetProgramInfoLog(this->_programId, logLength, &actualLogLength, log);

        if (actualLogLength > 0)
        {
            std::string logString = log;
            this->LogMessage(logString);
        }
        else
        {
            this->LogMessage("No error log returned.");
        }

        delete[] log;
    }

    void ShaderProgram::SetUniformVec4(const std::string& name, std::array<float, 4> data) const noexcept
    {
        static_assert(data.size() == 4);

        auto location = glGetUniformLocation(this->_programId, name.c_str());
        glUniform4fv(location, 1, data.data());
    }
}