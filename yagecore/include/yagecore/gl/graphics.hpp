/** ========================================================================
 *
 * @file graphics.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <string>

#include "GL/gl.h"
#include "GL/glext.h"
#include "yagecore/graphics/graphics.hpp"

namespace yc::gl
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics : public yc::graphics::graphics
{
private:
  std::vector<std::string> extensions;

  void cache_extensions();
  void print_extensions();
  bool has_extension(const char * extension);

public:
  /* function cache. Because we don't know what
   * version of OpenGL we are using */
  PFNGLCREATESHADERPROC            glCreateShader {};
  PFNGLSHADERSOURCEPROC            glShaderSource {};
  PFNGLCOMPILESHADERPROC           glCompileShader {};
  PFNGLGETSHADERIVPROC             glGetShaderiv {};
  PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog {};
  PFNGLDELETESHADERPROC            glDeleteShader {};
  PFNGLCREATEPROGRAMPROC           glCreateProgram {};
  PFNGLATTACHSHADERPROC            glAttachShader {};
  PFNGLLINKPROGRAMPROC             glLinkProgram {};
  PFNGLGETPROGRAMIVPROC            glGetProgramiv {};
  PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog {};
  PFNGLUSEPROGRAMPROC              glUseProgram {};
  PFNGLDELETEPROGRAMPROC           glDeleteProgram {};
  PFNGLGETPROGRAMBINARYPROC        glGetProgramBinary {};
  PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays {};
  PFNGLBUFFERDATAPROC              glBufferData {};
  PFNGLGENBUFFERSPROC              glGenBuffers {};
  PFNGLBINDVERTEXARRAYPROC         glBindVertexArray {};
  PFNGLBINDBUFFERPROC              glBindBuffer {};
  PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer {};
  PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray {};
  PFNGLDRAWARRAYSEXTPROC           glDrawArrays {};
  PFNGLSHADERBINARYPROC            glShaderBinary {};
  PFNGLPROGRAMBINARYPROC           glProgramBinary {};
  PFNGLACTIVETEXTUREPROC           glActiveTexture {};
  PFNGLBINDTEXTUREEXTPROC          glBindTexture {};
  PFNGLGENERATEMIPMAPPROC          glGenerateMipmap {};
  PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation {};
  PFNGLGETATTRIBLOCATIONPROC       glGetAttribLocation {};
  PFNGLUNIFORM1IPROC               glUniform1i {};
  PFNGLUNIFORM1FPROC               glUniform1f {};
  PFNGLUNIFORM2FPROC               glUniform2f {};
  PFNGLUNIFORM3FPROC               glUniform3f {};
  PFNGLUNIFORM4FPROC               glUniform4f {};
  PFNGLUNIFORMMATRIX2FVPROC        glUniformMatrix2fv {};
  PFNGLUNIFORMMATRIX3FVPROC        glUniformMatrix3fv {};
  PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv {};

  yc::error init(int bits_per_pixel, int width, int height);

  yc::graphics::shader * create_shader(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics) override;
  yc::error destroy_shader(yc::graphics::shader * shader) override;

  yc::graphics::node * create_node(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics) override;
  yc::error destroy_node(yc::graphics::node * node) override;

  yc::graphics::texture * create_texture(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics) override;
  yc::error destroy_texture(yc::graphics::texture * texture) override;

  bool supports_shader_language(const std::string & language) override
  {
    return language == "GLSL";
  }

  yc::error clear() override;
  yc::error flush() override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
