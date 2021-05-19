#ifndef PROJECTFARM_GRAPHICS_DEPENDENCIES_H
#define PROJECTFARM_GRAPHICS_DEPENDENCIES_H

#include "engine/platform.h"

#if defined(IS_IOS)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#define USE_OPENGL_ES

#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES

#else
#include <GL/glew.h>

#ifdef USE_OPENGL_ES
#undef USE_OPENGL_ES
#endif
#endif

#endif
