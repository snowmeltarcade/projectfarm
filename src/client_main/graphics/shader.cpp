#include <fstream>
#include <sstream>

#include "shader.h"

using namespace std::literals;

namespace projectfarm::graphics
{
    bool Shader::Load(const std::string& name) noexcept
    {
        auto shaderCode = ""s;

        if (auto code = this->GetShaderCode(name); !code)
        {
            this->LogMessage("Failed to get shader code for shader with name: " + name);
            return false;
        }
        else
        {
            shaderCode = code->first;
            this->_type = code->second;
        }

        GLenum createShaderType = this->_type == ShaderTypes::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

        this->_shaderId = glCreateShader(createShaderType);

        const char* sourceData = shaderCode.c_str();
        glShaderSource(this->_shaderId, 1, &sourceData, nullptr);

        glCompileShader(this->_shaderId);

        GLint compiledStatus = GL_FALSE;
        glGetShaderiv(this->_shaderId, GL_COMPILE_STATUS, &compiledStatus);
        if (compiledStatus != GL_TRUE)
        {
            this->LogMessage("Failed to compile shader with source:");
            this->LogMessage(shaderCode);
            this->LogErrors();
            return false;
        }

        return true;
    }

    void Shader::Destroy() noexcept
    {
        if (this->_shaderId == 0)
        {
            return;
        }

        glDeleteShader(this->_shaderId);
        this->_shaderId = 0;
    }

    void Shader::LogErrors() const noexcept
    {
        GLint logLength = 0;
        GLint actualLogLength = 0;

        glGetShaderiv(this->_shaderId, GL_INFO_LOG_LENGTH, &logLength);

        char* log = new char[logLength];

        glGetShaderInfoLog(this->_shaderId, logLength, &actualLogLength, log);

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

    std::optional<std::pair<std::string, ShaderTypes>> Shader::GetShaderCode(const std::string& name) const noexcept
    {
        auto path = this->_dataProvider->GetGraphicsShaderLocationByName(name);

#if defined(USE_OPENGL_ES)
        auto shaderFolder = "es";
#else
        auto shaderFolder = "core";
#endif

        auto segmentToReplace = "<>"s;

        auto pathString = path.u8string();
        path = pathString.replace(pathString.find(segmentToReplace),
                                  segmentToReplace.size(),
                                  shaderFolder);

        std::ifstream fp(path);
        if (!fp.is_open())
        {
            this->LogMessage("Failed to find shader with path: " + path.u8string());
            return {};
        }

        std::stringstream ss;
        ss << fp.rdbuf();

        auto type = path.extension() == ".frag" ? ShaderTypes::Fragment : ShaderTypes::Vertex;

        return { { ss.str(), type } };
    }
}
