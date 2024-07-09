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

#include <stdexcept>
#include <unordered_map>

#include "yagecore/gl/glplatform.hpp"
#include "yagecore/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
PFNGLCREATESHADERPROC            yc::gl::glCreateShader;
PFNGLSHADERSOURCEPROC            yc::gl::glShaderSource;
PFNGLCOMPILESHADERPROC           yc::gl::glCompileShader;
PFNGLGETSHADERIVPROC             yc::gl::glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC        yc::gl::glGetShaderInfoLog;
PFNGLDELETESHADERPROC            yc::gl::glDeleteShader;
PFNGLCREATEPROGRAMPROC           yc::gl::glCreateProgram;
PFNGLATTACHSHADERPROC            yc::gl::glAttachShader;
PFNGLLINKPROGRAMPROC             yc::gl::glLinkProgram;
PFNGLGETPROGRAMIVPROC            yc::gl::glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC       yc::gl::glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC              yc::gl::glUseProgram;
PFNGLDELETEPROGRAMPROC           yc::gl::glDeleteProgram;
PFNGLGETPROGRAMBINARYPROC        yc::gl::glGetProgramBinary;
PFNGLGENVERTEXARRAYSPROC         yc::gl::glGenVertexArrays;
PFNGLBUFFERDATAPROC              yc::gl::glBufferData;
PFNGLGENBUFFERSPROC              yc::gl::glGenBuffers;
PFNGLDELETEBUFFERSPROC           yc::gl::glDeleteBuffers;
PFNGLBINDVERTEXARRAYPROC         yc::gl::glBindVertexArray;
PFNGLBINDBUFFERPROC              yc::gl::glBindBuffer;
PFNGLVERTEXATTRIBPOINTERPROC     yc::gl::glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC yc::gl::glEnableVertexAttribArray;
PFNGLDRAWARRAYSEXTPROC           yc::gl::glDrawArrays;
yc::gl::PFNGLDRAWELEMENTSPROC    yc::gl::glDrawElements;
PFNGLSHADERBINARYPROC            yc::gl::glShaderBinary;
PFNGLPROGRAMBINARYPROC           yc::gl::glProgramBinary;
PFNGLACTIVETEXTUREPROC           yc::gl::glActiveTexture;
PFNGLBINDTEXTUREEXTPROC          yc::gl::glBindTexture;
PFNGLGENERATEMIPMAPPROC          yc::gl::glGenerateMipmap;
PFNGLGETUNIFORMLOCATIONPROC      yc::gl::glGetUniformLocation;
PFNGLGETATTRIBLOCATIONPROC       yc::gl::glGetAttribLocation;
PFNGLUNIFORM1IPROC               yc::gl::glUniform1i;
PFNGLUNIFORM1FPROC               yc::gl::glUniform1f;
PFNGLUNIFORM2FPROC               yc::gl::glUniform2f;
PFNGLUNIFORM3FPROC               yc::gl::glUniform3f;
PFNGLUNIFORM4FPROC               yc::gl::glUniform4f;
PFNGLUNIFORMMATRIX2FVPROC        yc::gl::glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC        yc::gl::glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC        yc::gl::glUniformMatrix4fv;
PFNGLDEBUGMESSAGECONTROLPROC     yc::gl::glDebugMessageControl;
PFNGLDEBUGMESSAGECALLBACKPROC    yc::gl::glDebugMessageCallback;

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
void yc::gl::gl_update_cache()
{
  // clang-format off
  yc::gl::glCreateShader            = reinterpret_cast<PFNGLCREATESHADERPROC>(gl_lookup("glCreateShader", true));
  yc::gl::glShaderSource            = reinterpret_cast<PFNGLSHADERSOURCEPROC>(gl_lookup("glShaderSource", true));
  yc::gl::glCompileShader           = reinterpret_cast<PFNGLCOMPILESHADERPROC>(gl_lookup("glCompileShader", true));
  yc::gl::glGetShaderiv             = reinterpret_cast<PFNGLGETSHADERIVPROC>(gl_lookup("glGetShaderiv", true));
  yc::gl::glGetShaderInfoLog        = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(gl_lookup("glGetShaderInfoLog", true));
  yc::gl::glDeleteShader            = reinterpret_cast<PFNGLDELETESHADERPROC>(gl_lookup("glDeleteShader", true));
  yc::gl::glCreateProgram           = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(gl_lookup("glCreateProgram", true));
  yc::gl::glAttachShader            = reinterpret_cast<PFNGLATTACHSHADERPROC>(gl_lookup("glAttachShader", true));
  yc::gl::glLinkProgram             = reinterpret_cast<PFNGLLINKPROGRAMPROC>(gl_lookup("glLinkProgram", true));
  yc::gl::glGetProgramiv            = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(gl_lookup("glGetProgramiv", true));
  yc::gl::glGetProgramInfoLog       = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(gl_lookup("glGetProgramInfoLog", true));
  yc::gl::glUseProgram              = reinterpret_cast<PFNGLUSEPROGRAMPROC>(gl_lookup("glUseProgram", true));
  yc::gl::glDeleteProgram           = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(gl_lookup("glDeleteProgram", true));
  yc::gl::glGetProgramBinary        = reinterpret_cast<PFNGLGETPROGRAMBINARYPROC>(gl_lookup("glGetProgramBinary", true));
  yc::gl::glGenVertexArrays         = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(gl_lookup("glGenVertexArrays", true));
  yc::gl::glBufferData              = reinterpret_cast<PFNGLBUFFERDATAPROC>(gl_lookup("glBufferData", true));
  yc::gl::glGenBuffers              = reinterpret_cast<PFNGLGENBUFFERSPROC>(gl_lookup("glGenBuffers", true));
  yc::gl::glDeleteBuffers           = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(gl_lookup("glDeleteBuffers", true));
  yc::gl::glBindVertexArray         = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(gl_lookup("glBindVertexArray", true));
  yc::gl::glBindBuffer              = reinterpret_cast<PFNGLBINDBUFFERPROC>(gl_lookup("glBindBuffer", true));
  yc::gl::glVertexAttribPointer     = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(gl_lookup("glVertexAttribPointer", true));
  yc::gl::glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(gl_lookup("glEnableVertexAttribArray", true));
  yc::gl::glDrawArrays              = reinterpret_cast<PFNGLDRAWARRAYSEXTPROC>(gl_lookup("glDrawArrays", true));
  yc::gl::glDrawElements            = reinterpret_cast<yc::gl::PFNGLDRAWELEMENTSPROC>(gl_lookup("glDrawElements", true));
  yc::gl::glShaderBinary            = reinterpret_cast<PFNGLSHADERBINARYPROC>(gl_lookup("glShaderBinary", true));
  yc::gl::glProgramBinary           = reinterpret_cast<PFNGLPROGRAMBINARYPROC>(gl_lookup("glProgramBinary", true));
  yc::gl::glActiveTexture           = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(gl_lookup("glActiveTexture", true));
  yc::gl::glBindTexture             = reinterpret_cast<PFNGLBINDTEXTUREEXTPROC>(gl_lookup("glBindTexture", true));
  yc::gl::glGenerateMipmap          = reinterpret_cast<PFNGLGENERATEMIPMAPPROC>(gl_lookup("glGenerateMipmap", true));
  yc::gl::glGetUniformLocation      = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(gl_lookup("glGetUniformLocation", true));
  yc::gl::glGetAttribLocation       = reinterpret_cast<PFNGLGETATTRIBLOCATIONPROC>(gl_lookup("glGetAttribLocation", true));
  yc::gl::glUniform1i               = reinterpret_cast<PFNGLUNIFORM1IPROC>(gl_lookup("glUniform1i", true));
  yc::gl::glUniform1f               = reinterpret_cast<PFNGLUNIFORM1FPROC>(gl_lookup("glUniform1f", true));
  yc::gl::glUniform2f               = reinterpret_cast<PFNGLUNIFORM2FPROC>(gl_lookup("glUniform2f", true));
  yc::gl::glUniform3f               = reinterpret_cast<PFNGLUNIFORM3FPROC>(gl_lookup("glUniform3f", true));
  yc::gl::glUniform4f               = reinterpret_cast<PFNGLUNIFORM4FPROC>(gl_lookup("glUniform4f", true));
  yc::gl::glUniformMatrix2fv        = reinterpret_cast<PFNGLUNIFORMMATRIX2FVPROC>(gl_lookup("glUniformMatrix2fv", true));
  yc::gl::glUniformMatrix3fv        = reinterpret_cast<PFNGLUNIFORMMATRIX3FVPROC>(gl_lookup("glUniformMatrix3fv", true));
  yc::gl::glUniformMatrix4fv        = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(gl_lookup("glUniformMatrix4fv", true));
  yc::gl::glDebugMessageControl     = reinterpret_cast<PFNGLDEBUGMESSAGECONTROLPROC>(gl_lookup("glDebugMessageControl", false));
  yc::gl::glDebugMessageCallback    = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(gl_lookup("glDebugMessageCallback", false));
  // clang-format on
}
