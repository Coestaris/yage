/** ========================================================================
 *
 * @file glplaftorm.cpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <GL/gl.h>
#if YAGE_PLATFORM_X11
#include <GL/glx.h>
#elif __WIN32__ || _WIN32
#include <GL/glext.h>
#endif

#include <functional>

#include <stdexcept>
#include <unordered_map>

#include "yagecore/gl/glplatform.hpp"
#include "yagecore/log.hpp"

#include "yagecore/gl/node.hpp"
#include "yagecore/graphics/aabb.hpp"
#include "yagecore/graphics/material.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
DECL_GL(PFNGLCREATESHADERPROC, yc::gl::glCreateShader);
DECL_GL(PFNGLSHADERSOURCEPROC, yc::gl::glShaderSource);
DECL_GL(PFNGLCOMPILESHADERPROC, yc::gl::glCompileShader);
DECL_GL(PFNGLGETSHADERIVPROC, yc::gl::glGetShaderiv);
DECL_GL(PFNGLGETSHADERINFOLOGPROC, yc::gl::glGetShaderInfoLog);
DECL_GL(PFNGLDELETESHADERPROC, yc::gl::glDeleteShader);
DECL_GL(PFNGLCREATEPROGRAMPROC, yc::gl::glCreateProgram);
DECL_GL(PFNGLATTACHSHADERPROC, yc::gl::glAttachShader);
DECL_GL(PFNGLLINKPROGRAMPROC, yc::gl::glLinkProgram);
DECL_GL(PFNGLGETPROGRAMIVPROC, yc::gl::glGetProgramiv);
DECL_GL(PFNGLGETPROGRAMINFOLOGPROC, yc::gl::glGetProgramInfoLog);
DECL_GL(PFNGLUSEPROGRAMPROC, yc::gl::glUseProgram);
DECL_GL(PFNGLDELETEPROGRAMPROC, yc::gl::glDeleteProgram);
DECL_GL(PFNGLGETPROGRAMBINARYPROC, yc::gl::glGetProgramBinary);
DECL_GL(PFNGLGENVERTEXARRAYSPROC, yc::gl::glGenVertexArrays);
DECL_GL(PFNGLDELETEVERTEXARRAYSPROC, yc::gl::glDeleteVertexArrays);
DECL_GL(PFNGLBUFFERDATAPROC, yc::gl::glBufferData);
DECL_GL(PFNGLGENBUFFERSPROC, yc::gl::glGenBuffers);
DECL_GL(PFNGLDELETEBUFFERSPROC, yc::gl::glDeleteBuffers);
DECL_GL(PFNGLBINDVERTEXARRAYPROC, yc::gl::glBindVertexArray);
DECL_GL(PFNGLBINDBUFFERPROC, yc::gl::glBindBuffer);
DECL_GL(PFNGLVERTEXATTRIBPOINTERPROC, yc::gl::glVertexAttribPointer);
DECL_GL(PFNGLENABLEVERTEXATTRIBARRAYPROC, yc::gl::glEnableVertexAttribArray);
DECL_GL(PFNGLDRAWARRAYSEXTPROC, yc::gl::glDrawArrays);
DECL_GL(yc::gl::PFNGLDRAWELEMENTSPROC, yc::gl::glDrawElements);
DECL_GL(PFNGLSHADERBINARYPROC, yc::gl::glShaderBinary);
DECL_GL(PFNGLPROGRAMBINARYPROC, yc::gl::glProgramBinary);
DECL_GL(PFNGLACTIVETEXTUREPROC, yc::gl::glActiveTexture);
DECL_GL(PFNGLBINDTEXTUREEXTPROC, yc::gl::glBindTexture);
DECL_GL(PFNGLGENERATEMIPMAPPROC, yc::gl::glGenerateMipmap);
DECL_GL(PFNGLGETUNIFORMLOCATIONPROC, yc::gl::glGetUniformLocation);
DECL_GL(PFNGLGETATTRIBLOCATIONPROC, yc::gl::glGetAttribLocation);
DECL_GL(PFNGLUNIFORM1IPROC, yc::gl::glUniform1i);
DECL_GL(PFNGLUNIFORM1FPROC, yc::gl::glUniform1f);
DECL_GL(PFNGLUNIFORM2FPROC, yc::gl::glUniform2f);
DECL_GL(PFNGLUNIFORM3FPROC, yc::gl::glUniform3f);
DECL_GL(PFNGLUNIFORM4FPROC, yc::gl::glUniform4f);
DECL_GL(PFNGLUNIFORMMATRIX2FVPROC, yc::gl::glUniformMatrix2fv);
DECL_GL(PFNGLUNIFORMMATRIX3FVPROC, yc::gl::glUniformMatrix3fv);
DECL_GL(PFNGLUNIFORMMATRIX4FVPROC, yc::gl::glUniformMatrix4fv);
DECL_GL(PFNGLDEBUGMESSAGECONTROLPROC, yc::gl::glDebugMessageControl);
DECL_GL(PFNGLDEBUGMESSAGECALLBACKPROC, yc::gl::glDebugMessageCallback);

static std::unordered_map<std::string, void *> cache;

/* Static functions ========================================================= */
void * gl_lookup(const char * func, bool required)
{
  if (cache.find(func) != cache.end())
  {
    return cache[func];
  }

#if WIN32
  auto proc = reinterpret_cast<void *>(
        wglGetProcAddress(reinterpret_cast<LPCSTR>(func)));
#else
  auto proc = reinterpret_cast<void *>(
        glXGetProcAddress(reinterpret_cast<const GLubyte *>(func)));
#endif

  if (!proc)
  {
    if (required)
    {
      throw std::runtime_error("Function " + std::string(func) + " not found");
    }
    else
    {
      logging(LOG_ERROR, "Function %s not found", func);
    }

    return nullptr;
  }

  logging(LOG_DEBUG, "Function %s found: %p", func, proc);
  cache[func] = proc;
  return proc;
}

/* Shared functions ========================================================= */
/* Helper, to get std::function arguments */
template <typename T> struct DDDDD;

// Specialization for std::function
template <typename R, typename... Args>
struct DDDDD<std::function<R(Args...)>>
{
  using result_type = R;
  using arg_types   = std::tuple<Args...>;
};

#if DEEP_GL_DEBUG
#define assign_gl_function(__type, __func_name)                                \
  static auto __func_name##func_addr = reinterpret_cast<                       \
        typename DDDDD<__type>::result_type (*)(                   \
              typename DDDDD<__type>::arg_types...)>(              \
        gl_lookup(#__func_name, true));                                        \
  yc::gl::__func_name = [](auto && ...args) -> typename __type::result_type {  \
    if constexpr (!std::is_same_v<typename __type::result_type, void>)         \
    {                                                                          \
      return __func_name##func_addr(std::forward<decltype(args)>(args)...);    \
    }                                                                          \
    else                                                                       \
    {                                                                          \
      __func_name##func_addr(std::forward<decltype(args)>(args)...);         \
    }                                                                          \
  };
#else
template <typename Func> Func assign_gl_function(const char * func_name)
{
  return reinterpret_cast<Func>(gl_lookup(func_name, true));
}
#endif

void yc::gl::gl_update_cache()
{
#define __getter(__var) assign_gl_function(decltype(yc::gl::__var), __var);
  __getter(glCreateShader);
  __getter(glShaderSource);
  __getter(glCompileShader);
  __getter(glGetShaderiv);
  __getter(glGetShaderInfoLog);
  __getter(glDeleteShader);
  __getter(glCreateProgram);
  __getter(glAttachShader);
  __getter(glLinkProgram);
  __getter(glGetProgramiv);
  __getter(glGetProgramInfoLog);
  __getter(glUseProgram);
  __getter(glDeleteProgram);
  __getter(glGetProgramBinary);
  __getter(glDeleteVertexArrays);
  __getter(glGenVertexArrays);
  __getter(glBufferData);
  __getter(glGenBuffers);
  __getter(glDeleteBuffers);
  __getter(glBindVertexArray);
  __getter(glBindBuffer);
  __getter(glVertexAttribPointer);
  __getter(glEnableVertexAttribArray);
  __getter(glDrawArrays);
  __getter(glDrawElements);
  __getter(glShaderBinary);
  __getter(glProgramBinary);
  __getter(glActiveTexture);
  __getter(glBindTexture);
  __getter(glGenerateMipmap);
  __getter(glGetUniformLocation);
  __getter(glGetAttribLocation);
  __getter(glUniform1i);
  __getter(glUniform1f);
  __getter(glUniform2f);
  __getter(glUniform3f);
  __getter(glUniform4f);
  __getter(glUniformMatrix2fv);
  __getter(glUniformMatrix3fv);
  __getter(glUniformMatrix4fv);
  __getter(glDebugMessageControl);
  __getter(glDebugMessageCallback);
}

std::shared_ptr<yc::gl::mesh_buffer>
yc::gl::gl_cube(const std::shared_ptr<yc::gl::graphics> & ctx)
{
  // Cube 1x1x1, centered on origin
  static constexpr GLfloat vertices[] = {
    // Vertex positions
    -0.5f, -0.5f, -0.5f, // 0
    0.5f,  -0.5f, -0.5f, // 1
    0.5f,  0.5f,  -0.5f, // 2
    -0.5f, 0.5f,  -0.5f, // 3
    -0.5f, -0.5f, 0.5f,  // 4
    0.5f,  -0.5f, 0.5f,  // 5
    0.5f,  0.5f,  0.5f,  // 6
    -0.5f, 0.5f,  0.5f   // 7
  };

  static constexpr GLushort elements[] = { // Front face
                                           0, 1, 1, 2, 2, 3, 3, 0,
                                           // Back face
                                           4, 5, 5, 6, 6, 7, 7, 4,
                                           // Connecting edges
                                           0, 4, 1, 5, 2, 6, 3, 7
  };

  /* Create Vertex Array Object */
  GLuint vao, vbo, ebo;
  yc::gl::glGenVertexArrays(1, &vao);
  yc::gl::glGenBuffers(1, &vbo);
  yc::gl::glGenBuffers(1, &ebo);

  /* Upload data to GPU */
  yc::gl::glBindVertexArray(vao);
  yc::gl::glBindBuffer(GL_ARRAY_BUFFER, vbo);
  yc::gl::glBufferData(
        GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  /* Upload indices to GPU */
  yc::gl::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  yc::gl::glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  /* Enable and set the vertex attribute pointer */
  yc::gl::glEnableVertexAttribArray(0);
  yc::gl::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  yc::graphics::aabb aabb({ -0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f });
  mesh_buffer::indicies_info indicies
        = { .ebo = ebo, .type = GL_UNSIGNED_SHORT, .offset = 0 };
  mesh_buffer::vertices_info vertices_info = {
    .vbo = vbo, .vao = vao, .type = GL_LINES, .count = 24, .aabb = aabb
  };
  return std::make_shared<mesh_buffer>(
        ctx, vertices_info, indicies, yc::graphics::material::basic_material());
}
