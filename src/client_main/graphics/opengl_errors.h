#ifndef PROJECTFARM_OPENGL_ERRORS_H
#define PROJECTFARM_OPENGL_ERRORS_H

#include <memory>

namespace projectfarm::graphics
{
    // this can ONLY be called after GLEW has been initialized, as `glGetError` requires GLEW to set it up
    [[nodiscard]]
    bool CheckOpenGLErrorInternal(const char* fileName, uint32_t lineNumber);

#ifdef DEBUG
#define CHECK_OPENGL_ERROR \
if (!projectfarm::graphics::CheckOpenGLErrorInternal(__FILE__, __LINE__)) \
{ \
    shared::api::logging::Log("OpenGL Error."); \
    return false; \
}
#define CHECK_OPENGL_ERROR_NO_RETURN \
if (!projectfarm::graphics::CheckOpenGLErrorInternal(__FILE__, __LINE__)) \
{ \
    shared::api::logging::Log("OpenGL Error."); \
}
#else
#define CHECK_OPENGL_ERROR
#endif

}

#endif
