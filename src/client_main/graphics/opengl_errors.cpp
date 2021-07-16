#include <sstream>

#include "opengl_errors.h"
#include "graphics_dependencies.h"
#include "api/logging/logging.h"

namespace projectfarm::graphics
{
    bool CheckOpenGLErrorInternal(const char* fileName, uint32_t lineNumber)
    {
        GLenum error = GL_NO_ERROR;
        bool hasError = false;

        while ((error = glGetError()) != GL_NO_ERROR)
        {
            std::string errorString;

            switch (error)
            {
                case GL_INVALID_ENUM:
                {
                    errorString = "GL_INVALID_ENUM";
                    break;
                }
                case GL_INVALID_VALUE:
                {
                    errorString = "GL_INVALID_VALUE";
                    break;
                }
                case GL_INVALID_OPERATION:
                {
                    errorString = "GL_INVALID_OPERATION";
                    break;
                }
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                {
                    errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                    break;
                }
                case GL_OUT_OF_MEMORY:
                {
                    errorString = "GL_OUT_OF_MEMORY";
                    break;
                }
#if !defined(IS_IOS)
                case GL_STACK_UNDERFLOW:
                {
                    errorString = "GL_STACK_UNDERFLOW";
                    break;
                }
                case GL_STACK_OVERFLOW:
                {
                    errorString = "GL_STACK_OVERFLOW";
                    break;
                }
#endif
                default:
                {
                    errorString = "Unknown error";
                    break;
                }
            }

            std::stringstream ss;
            ss << "**********" << std::endl;
            ss << "OpenGL Error" << std::endl;
            ss << "File: " << fileName << ":" << lineNumber << std::endl;
            ss << "Number: " << static_cast<uint32_t>(error) << std::endl;
            ss << "String: " << errorString << std::endl;
            ss << "**********" << std::endl;

            shared::api::logging::Log(ss.str());

            hasError = true;
        }

        return !hasError;
    }
}
