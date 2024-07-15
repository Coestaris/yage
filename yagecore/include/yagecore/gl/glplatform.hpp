/** ========================================================================
 *
 * @file glplatform.hpp
 * @date 15-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <memory>

namespace yc::gl
{

class mesh_buffer;
class graphics;

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
#if DEEP_GL_DEBUG
// Template to extract function signature from a typedef
template <typename T> struct FunctionSignature;
template <typename R, typename... Args>
struct FunctionSignature<R(APIENTRY *)(Args...)>
{
  using Type = std::function<R(Args...)>;
};

// Macro to define std::function from a C-style typedef
#define DECL_GL(typedef_name, std_function_name)                               \
  yc::gl::FunctionSignature<typedef_name>::Type std_function_name;
#else
#define DECL_GL(typedef_name, std_function_name) typedef_name std_function_name;
#endif

using PFNGLDRAWELEMENTSPROC
      = void (*)(GLenum mode, GLsizei count, GLenum type, const void * indices);

/* Variables ================================================================ */

/* function cache. Because we don't know what
 * version of OpenGL we are using */
extern DECL_GL(PFNGLCREATESHADERPROC, glCreateShader);
extern DECL_GL(PFNGLSHADERSOURCEPROC, glShaderSource);
extern DECL_GL(PFNGLCOMPILESHADERPROC, glCompileShader);
extern DECL_GL(PFNGLGETSHADERIVPROC, glGetShaderiv);
extern DECL_GL(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
extern DECL_GL(PFNGLDELETESHADERPROC, glDeleteShader);
extern DECL_GL(PFNGLCREATEPROGRAMPROC, glCreateProgram);
extern DECL_GL(PFNGLATTACHSHADERPROC, glAttachShader);
extern DECL_GL(PFNGLLINKPROGRAMPROC, glLinkProgram);
extern DECL_GL(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
extern DECL_GL(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
extern DECL_GL(PFNGLUSEPROGRAMPROC, glUseProgram);
extern DECL_GL(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
extern DECL_GL(PFNGLGETPROGRAMBINARYPROC, glGetProgramBinary);
extern DECL_GL(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
extern DECL_GL(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
extern DECL_GL(PFNGLBUFFERDATAPROC, glBufferData);
extern DECL_GL(PFNGLGENBUFFERSPROC, glGenBuffers);
extern DECL_GL(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
extern DECL_GL(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
extern DECL_GL(PFNGLBINDBUFFERPROC, glBindBuffer);
extern DECL_GL(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
extern DECL_GL(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
extern DECL_GL(PFNGLDRAWARRAYSEXTPROC, glDrawArrays);
extern DECL_GL(PFNGLDRAWELEMENTSPROC, glDrawElements);
extern DECL_GL(PFNGLSHADERBINARYPROC, glShaderBinary);
extern DECL_GL(PFNGLPROGRAMBINARYPROC, glProgramBinary);
extern DECL_GL(PFNGLACTIVETEXTUREPROC, glActiveTexture);
extern DECL_GL(PFNGLBINDTEXTUREEXTPROC, glBindTexture);
extern DECL_GL(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);
extern DECL_GL(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
extern DECL_GL(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
extern DECL_GL(PFNGLUNIFORM1IPROC, glUniform1i);
extern DECL_GL(PFNGLUNIFORM1FPROC, glUniform1f);
extern DECL_GL(PFNGLUNIFORM2FPROC, glUniform2f);
extern DECL_GL(PFNGLUNIFORM3FPROC, glUniform3f);
extern DECL_GL(PFNGLUNIFORM4FPROC, glUniform4f);
extern DECL_GL(PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv);
extern DECL_GL(PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv);
extern DECL_GL(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
extern DECL_GL(PFNGLDEBUGMESSAGECONTROLPROC, glDebugMessageControl);
extern DECL_GL(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback);

/* Prototypes =============================================================== */
void gl_update_cache();

/** Little helper function to create a cube mesh.
 * Attention: Created mesh, is not managed by any node, so it should be
 * unloaded manually. */
std::shared_ptr<yc::gl::mesh_buffer>
gl_cube(const std::shared_ptr<yc::gl::graphics> & ctx);

}
