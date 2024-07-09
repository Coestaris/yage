/** ========================================================================
 *
 * @file glplatform.hpp
 * @date 15-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */

namespace yc::gl
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
using PFNGLDRAWELEMENTSPROC
      = void (*)(GLenum mode, GLsizei count, GLenum type, const void * indices);

/* Variables ================================================================ */
/* function cache. Because we don't know what
 * version of OpenGL we are using */
extern PFNGLCREATESHADERPROC            glCreateShader;
extern PFNGLSHADERSOURCEPROC            glShaderSource;
extern PFNGLCOMPILESHADERPROC           glCompileShader;
extern PFNGLGETSHADERIVPROC             glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC            glDeleteShader;
extern PFNGLCREATEPROGRAMPROC           glCreateProgram;
extern PFNGLATTACHSHADERPROC            glAttachShader;
extern PFNGLLINKPROGRAMPROC             glLinkProgram;
extern PFNGLGETPROGRAMIVPROC            glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC              glUseProgram;
extern PFNGLDELETEPROGRAMPROC           glDeleteProgram;
extern PFNGLGETPROGRAMBINARYPROC        glGetProgramBinary;
extern PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
extern PFNGLBUFFERDATAPROC              glBufferData;
extern PFNGLGENBUFFERSPROC              glGenBuffers;
extern PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
extern PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
extern PFNGLBINDBUFFERPROC              glBindBuffer;
extern PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLDRAWARRAYSEXTPROC           glDrawArrays;
extern PFNGLDRAWELEMENTSPROC            glDrawElements;
extern PFNGLSHADERBINARYPROC            glShaderBinary;
extern PFNGLPROGRAMBINARYPROC           glProgramBinary;
extern PFNGLACTIVETEXTUREPROC           glActiveTexture;
extern PFNGLBINDTEXTUREEXTPROC          glBindTexture;
extern PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;
extern PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
extern PFNGLGETATTRIBLOCATIONPROC       glGetAttribLocation;
extern PFNGLUNIFORM1IPROC               glUniform1i;
extern PFNGLUNIFORM1FPROC               glUniform1f;
extern PFNGLUNIFORM2FPROC               glUniform2f;
extern PFNGLUNIFORM3FPROC               glUniform3f;
extern PFNGLUNIFORM4FPROC               glUniform4f;
extern PFNGLUNIFORMMATRIX2FVPROC        glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC        glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
extern PFNGLDEBUGMESSAGECONTROLPROC     glDebugMessageControl;
extern PFNGLDEBUGMESSAGECALLBACKPROC    glDebugMessageCallback;

/* Prototypes =============================================================== */
void gl_update_cache();

}
