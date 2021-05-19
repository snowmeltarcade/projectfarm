#include <fstream>
#include <nlohmann/json.hpp>

#include "graphics.h"
#include "material.h"

namespace projectfarm::graphics
{
    bool Material::LoadFromFile(const std::filesystem::path& path) noexcept
    {
        // nlohmann::json can throw exceptions
        try
        {
            std::ifstream file(path);

            if (!file.is_open())
            {
                this->LogMessage("Failed to open material file: " + path.u8string());
                return false;
            }

            nlohmann::json jsonFile;
            file >> jsonFile;

            const auto& shaders = jsonFile["shaders"];

            this->_shaderProgram.SetLogger(this->_logger);
            if (!this->_shaderProgram.Create())
            {
                this->LogMessage("Failed to create shader program.");
                return false;
            }

            for (const auto& shader : shaders)
            {
                auto vertex = shader["vertex"].get<std::string>();
                auto fragment = shader["fragment"].get<std::string>();

                if (!this->LoadShader(vertex, fragment))
                {
                    this->LogMessage("Failed to load shaders.");
                    return false;
                }
            }

            if (!this->_shaderProgram.Link())
            {
                this->LogMessage("Failed to link shader program.");
                return false;
            }

            auto colorName = jsonFile["color"].get<std::string>();
            if (auto c = shared::graphics::colors::FromString(colorName); c)
            {
                this->SetColor(*c);
            }

            this->_numberOfTextures = jsonFile["numberOfTextures"].get<uint32_t>();

            // bind the texture uniform locations
            this->Bind();
            for (auto i = 0u; i < this->_numberOfTextures; ++i)
            {
                auto name = "texture" + std::to_string(i);
                auto location = glGetUniformLocation(this->_shaderProgram.GetProgramId(), name.c_str());
                glUniform1i(location, i);
            }
            this->Unbind();
        }
        catch (const std::exception& ex)
        {
            this->LogMessage("Failed to load material from path: " + path.u8string() +
                             "with exception: " + ex.what());

            return false;
        }

        return true;
    }

    void Material::Destroy() noexcept
    {
        this->_shaderProgram.Destroy();
    }

    bool Material::LoadShader(const std::string& vertexShaderName, const std::string& fragmentShaderName) noexcept
    {
        if (auto vertexShader = this->GetGraphics()->GetVertexShaderByName(vertexShaderName); !vertexShader)
        {
            this->LogMessage("Failed to get vertex shader with name: " + vertexShaderName);
            return false;
        }
        else
        {
            if (!this->_shaderProgram.AttachShader(vertexShader))
            {
                this->LogMessage("Failed to attach vertex shader with name: " + vertexShaderName);
                return false;
            }
        }

        if (auto fragmentShader = this->GetGraphics()->GetFragmentShaderByName(fragmentShaderName); !fragmentShader)
        {
            this->LogMessage("Failed to get fragment shader with name: " + fragmentShaderName);
            return false;
        }
        else
        {
            if (!this->_shaderProgram.AttachShader(fragmentShader))
            {
                this->LogMessage("Failed to attach fragment shader with name: " + fragmentShaderName);
                return false;
            }
        }

        return true;
    }

    void Material::SetColor(const shared::graphics::colors::Color& color) noexcept
    {
        this->_color = color;

        this->_shaderProgram.Use();
        this->_shaderProgram.SetUniformVec4("color",
                                            {
                                                    this->_color.r / 255.0f,
                                                    this->_color.g / 255.0f,
                                                    this->_color.b / 255.0f,
                                                    this->_color.a / 255.0f,
                                            });
    }
}